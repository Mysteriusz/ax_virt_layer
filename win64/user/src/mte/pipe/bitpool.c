#include "bitpool.h"

axres bitpool_create(
	_in const u32				capacity, 
	_in struct bitpool_desc_perc 		perc,
	_out bitpool_desc			**buf
){
	if (capacity == 0){
		return AX_INV_ARG;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	/*bitpool_desc *bitpool =
		axmalloc(sizeof(bitpool_desc));

	bitpool->capacity = capacity;
	res = sync_map_init(capacity, &bitpool->smap);
	axcheck_r(res, res, axfree(bitpool));*/ // TODO: Change the return code

	/*res = bitpool_range_populate(&bitpool->smap, perc, &bitpool->ranges);
	axcheck_r(res, res, { // TODO: Change the return code
	 	sync_map_disp(&bitpool->smap);
		axfree(bitpool);
	});*/

	//*buf = bitpool;

	return AX_SUCC;
}

void bitpool_delete(
	_in bitpool_desc	*bitpool
){
	if (bitpool == nullptr){
		return;
	}
	sync_map_disp(&bitpool->smap);
	axfree(bitpool);
}

axres bitpool_range_populate(
	_in sync_map_desc			*smap,
	_in struct bitpool_desc_perc 		perc,
	_in_out struct bitpool_range_desc 	*buf
){
	if (smap == nullptr){
		return AX_INV_ARG;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	if (bitpool_desc_perc_sum(perc) != 100){
		return AX_INV_DATA;
	}

	u8 smap_bit_n = smap->size * 8;

	/*
	 	Process real priority range
	*/
	buf->real.bit_n = (perc.real_perc / 100) * smap_bit_n;
	buf->real.from = (smap_bit_n - buf->real.bit_n) / 8;
	io_i64(buf->real.bit_n);
	io_i64(buf->real.from);

	/*
	 	Process high priority range
	*/
	/*
	 	Process med priority range
	*/
	/*
	 	Process low priority range
	*/

	return AX_SUCC;
}

