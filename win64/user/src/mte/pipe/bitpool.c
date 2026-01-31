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

	// Initialize the sync map
	res = sync_map_init(bucket_count, &bitpool->smap_desc);
	axcheck_r(res, res, axfree(bitpool)); // TODO: Change the return code

	// Calculate priority ranges given percentage
	res = bitpool_range_populate(&bitpool->smap, perc, &bitpool->ranges);
	axcheck_r(res, res, { // TODO: Change the return code
	 	sync_map_disp(&bitpool->smap_desc);
		axfree(bitpool);
	});

	// Allocate the buffer
	bitpool->base = axmalloc(bucket_size * bucket_count);

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
	if (bitpool->base != nullptr){
		axfree(bitpool->base);
	}

	axfree(bitpool);
}

axres bitpool_range_populate(
	_in sync_map_ref			*smap,
	_in struct bitpool_perc_desc 		perc,
	_in_out struct bitpool_range_desc 	*buf
){
	if (smap == nullptr){
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

	u32 smap_bit_n = smap->size;
	u32 taken = 0;

	/*
	 	Process real priority range
	*/
	buf->real.bit_n = round(((double)perc.real_perc / 100) * smap_bit_n);
	buf->real.from = smap_bit_n - buf->real.bit_n;

	taken += buf->real.bit_n;

	/*
	 	Process high priority range
	*/
	buf->high.bit_n = round(((double)perc.high_perc / 100) * smap_bit_n);
	buf->high.from = smap_bit_n - buf->high.bit_n - taken - 1;

	taken += buf->high.bit_n;
	/*
	 	Process med priority range
	*/
	buf->med.bit_n = round(((double)perc.med_perc / 100) * smap_bit_n);
	buf->med.from = smap_bit_n - buf->med.bit_n - taken - 1;

	taken += buf->med.bit_n;

	/*
	 	Process low priority range
	*/
	buf->low.bit_n = round(((double)(perc.low_perc + low_add) / 100) * smap_bit_n);
	buf->low.from = smap_bit_n - buf->low.bit_n - taken - 1;

	taken += buf->low.bit_n;

	if (taken != smap_bit_n){
		memset(buf, 0, sizeof(struct bitpool_range_desc));
		return AX_UNK_ERR; // TODO: Change the error code
	}

	return AX_SUCC;
}

bool bitpool_prior_load(
	_in bitpool_desc	*bitpool,
	_in enum bitpool_prior  prior
){
	if (bitpool == nullptr){
		return false;
	}
	
	bitpool_prior_range *range = 
		_bitpool_range_from_prior(&bitpool->ranges, prior);

	if (range == nullptr
	|| range->bit_n == 0){
		return false;
	}

	return true;
}

