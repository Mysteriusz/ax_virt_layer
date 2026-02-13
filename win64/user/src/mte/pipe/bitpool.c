#include <math.h>

#include "bitpool.h"

axres bitpool_create(
	_in const u32				bucket_count, 
	_in const u32				bucket_size, // Size in bytes
	_in bitpool_perc_desc 			perc,
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
	res = bitpool_range_populate(&bitpool->smap_desc, perc, bitpool->ranges);
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
	_in bitpool_perc_desc 			perc,
	_in_out bitpool_prior_range 		buf[4]
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

	/*
	 	[ranges[BLOW_PERC]] has to be the highest
	*/
	if (perc[3] < perc[0]
	|| perc[3] < perc[1]
	|| perc[3] < perc[2]){
		return AX_INV_DATA;
	}

	u32 smap_bit_n = smap_desc->size * 64;
	u32 taken = 0;

	/*
	 	Map all priority ranges
	*/
	for (u8 i = 0; i < 4; i++){
		buf[i].bit_n = round(((double)perc[i] / 100) * smap_bit_n);
		buf[i].bit_i = smap_bit_n - buf[i].bit_n - taken;
		taken += buf[i].bit_n;
	}

	/*
	 	Calculate disposition for each priority per 1 Low bit

		Example:
		For queue percentage disposition
		6 bits -> Real
		10 bits -> High
		16 bits -> Med
		32 bits -> Low

		For one Low there will be 5 Real (32 / 6)
		For one Low there will be 3 High (32 / 10)
		For one Low there will be 2 Med (32 / 16)
	*/
	for (u8 i = 0; i < 3; i++){
		*(u32*)&buf[i].bit_disp = buf[3].bit_n / buf[i].bit_n;
	}
	*(u32*)&buf[3].bit_disp = 1;

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
		&bitpool->ranges[_bitpool_prior_to_presence(bitpool, prior)];

	if (range == nullptr
	|| range->bit_n == 0){
		res.code = AX_INV_DATA;
		return res;
	}

	/*
		Block and signal first possible bit
		from [range->bit_i] to [range->bit_n]
	*/

	u32 index = 0;
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
		_bitpool_index_to_bucket(bitpool, index),
		bitpool->bucket_size);

	// Directly compute the presence bit based on the memory layout of the ranges

	// Add presence at this priority
	atomic_fetch_add_explicit(
		&bitpool->presence[_bitpool_prior_to_presence(bitpool, prior)],
		1,
		memory_order_seq_cst);

	// Write index to result buffer
	res.index = index;
	return res;
}

void bitpool_prior_unload(
	_in bitpool_desc	*bitpool,
	_in u32			index
){
	if (bitpool == nullptr 
	|| bitpool->bucket_count <= index){
		return;
	}

	// Signal the bucket index to off
	sync_map_sigoff(bitpool->smap_desc, index);

	// Subtract from presence value
	atomic_fetch_sub_explicit(
		&bitpool->presence[_bitpool_index_to_presence(bitpool, index)],
		1,
		memory_order_acq_rel);
}

struct bitpool_prior_unload_disp_res bitpool_prior_unload_disp(
	_in bitpool_desc	*bitpool,
	_in_out u32 		disp_presence[4]
){
	struct bitpool_prior_unload_disp_res res = {
		.succ = false,
		.bucket_i = 0,
		.presence_i = 0
	};

	if (bitpool == nullptr){
		return res;
	}

	u8 i = 0;
	for (; i < 3; i++){
		if (atomic_load_explicit(&bitpool->presence[i], memory_order_acquire) > 0){
			if (disp_presence[i] < bitpool->ranges[i].bit_disp){
				break;
			}
			disp_presence[i] = 0;
		}
		
	}	

	u32 bucket_i = 0;
	if (!sync_map_unsig_first(
		bitpool->ranges[i].bit_i,
		bitpool->ranges[i].bit_i + bitpool->ranges[i].bit_n,
		&bitpool->smap_desc,
		&bucket_i)
	){
		return res;
	}

	// Sub after unloading indexed (bucket_i) bucket
	atomic_fetch_sub_explicit(
		&bitpool->presence[i],
		1,
		memory_order_release);

	res.presence_i = i;
	res.bucket_i = bucket_i;
	res.succ = true;
	return res;
}

