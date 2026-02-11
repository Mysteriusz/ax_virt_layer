#include <math.h>

#include "bitpool.h"

axres bitpool_create(
	_in const u32				bucket_count, 
	_in const u32				bucket_size, // Size in bytes
	_in struct bitpool_perc_desc 		perc,
	_out bitpool_desc			**buf
){
	if (bucket_count == 0
	|| bucket_size == 0){
		return AX_INV_ARG;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	bitpool_desc *bitpool =
		axmalloc(sizeof(bitpool_desc));

	bitpool->bucket_count = bucket_count;
	bitpool->bucket_size = bucket_size;

	bitpool->qload = _bitpool_bucket_func(bucket_size);
	_mm_prefetch(bitpool->qload, _MM_HINT_T0);

	// Initialize the sync map
	res = sync_map_init(bucket_count, &bitpool->smap_desc);
	axcheck_r(res, res, axfree(bitpool)); // TODO: Change the return code

	// Calculate priority ranges given percentage
	res = bitpool_range_populate(&bitpool->smap_desc, perc, &bitpool->ranges);
	axcheck_r(res, res, { // TODO: Change the return code
	 	sync_map_disp(&bitpool->smap_desc);
		axfree(bitpool);
	});

	// Allocate the buffer
	bitpool->bucket_base
		= axmalloc(bucket_size * bucket_count);

	*buf = bitpool;

	return AX_SUCC;
}

void bitpool_delete(
	_in bitpool_desc	*bitpool
){
	if (bitpool == nullptr){
		return;
	}
	
	sync_map_disp(&bitpool->smap_desc);
	if (bitpool->bucket_base != nullptr){
		axfree(bitpool->bucket_base);
	}

	axfree(bitpool);
}

axres bitpool_range_populate(
	_in sync_map_desc			*smap_desc,
	_in struct bitpool_perc_desc 		perc,
	_in_out struct bitpool_range_desc 	*buf
){
	if (smap_desc == nullptr){
		return AX_INV_ARG;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	u32 sum = bitpool_perc_desc_sum(perc);
	if (sum > 100){
		return AX_INV_DATA;
	}

	// If percentage sum is smaller than 100 then rest is treated as low_perc
	u32 low_add = 100 - sum;

	u32 smap_bit_n = smap_desc->size * 64;
	u32 taken = 0;

	/*
	 	Process real priority range
	*/
	buf->real.bit_n = round(((double)perc.real_perc / 100) * smap_bit_n);
	buf->real.bit_i = smap_bit_n - buf->real.bit_n;

	taken += buf->real.bit_n;

	/*
	 	Process high priority range
	*/
	buf->high.bit_n = round(((double)perc.high_perc / 100) * smap_bit_n);
	buf->high.bit_i = smap_bit_n - buf->high.bit_n - taken;

	taken += buf->high.bit_n;
	/*
	 	Process med priority range
	*/
	buf->med.bit_n = round(((double)perc.med_perc / 100) * smap_bit_n);
	buf->med.bit_i = smap_bit_n - buf->med.bit_n - taken;

	taken += buf->med.bit_n;

	/*
	 	Process low priority range
	*/
	buf->low.bit_n = round(((double)(perc.low_perc + low_add) / 100) * smap_bit_n);
	buf->low.bit_i = smap_bit_n - buf->low.bit_n - taken;
	taken += buf->low.bit_n;

	if (taken != smap_bit_n){
		memset(buf, 0, sizeof(struct bitpool_range_desc));
		return AX_UNK_ERR; // TODO: Change the error code
	}

	return AX_SUCC;
}

struct bitpool_prior_load_res bitpool_prior_load(
	_in bitpool_desc	*bitpool,
	_in void		*bucket,
	_in enum mte_prior  	prior
){
	struct bitpool_prior_load_res res = {
		.code = AX_SUCC,
		.index = 0
	};

	if (bitpool == nullptr
	|| bucket == nullptr){
		res.code = AX_INV_ARG;
		return res;
	}

	// Convert priority to range pointer of the [*bitpool]
	bitpool_prior_range *range = 
		_bitpool_range_from_prior(&bitpool->ranges, prior);

	if (range == nullptr
	|| range->bit_n == 0){
		res.code = AX_INV_DATA;
		return res;
	}

	u32 index = 0;
	// Signal first possible bit from index (if possible)
	while (!sync_map_sig_first(
		range->bit_i,
		range->bit_i + range->bit_n,
		&bitpool->smap_desc,
		&index)
	){
		_mm_pause();
	}

	/*
	 	Chceck if bitpool qload is not corrupted
	*/
	asrt(bitpool->qload != nullptr);

	/*
	 	Load the signaled bucket with data using the quick_load function
		This function depends on the [bucket_size] parameter of [bitpool_create]
		It is recommended to use any of the BITPOOL_BUCKET_* for SIMD copy 
	*/
	bitpool->qload(
		bucket,
		offp(bitpool->bucket_base, bitpool->bucket_size * index),
		bitpool->bucket_size);

	// Directly compute the presence bit based on the memory layout of the ranges
	_Atomic(u32) *pres_ptr = &bitpool->presence[((u64)range - (u64)&bitpool->ranges) / sizeof(bitpool_prior_range) - 1];

	// Add presence at this priority
	atomic_fetch_add_explicit(pres_ptr, 1, memory_order_seq_cst);

	// Write index to result buffer
	res.index = index;
	return res;
}

void bitpool_prior_unload(
	_in bitpool_desc	*bitpool
)
{
}

