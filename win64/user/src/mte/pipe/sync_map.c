#include "sync_map.h"

axres sync_map_init(
	_in u32 		size,
	_in_out sync_map_desc	*buf
){
	if (size == 0){
		return AX_INV_ARG;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	sync_map_desc temp_smap = (sync_map_desc){
		.size = size,
		.map = axmalloc(size * sizeof(u64)),
	};
	memcpy(buf, &temp_smap, sizeof(sync_map_desc));

	return AX_SUCC;
}

void sync_map_disp(
	_in sync_map_desc	*smap
){
	if (smap == nullptr){
		return;
	}

	axfree(smap->map);
	memset(smap, 0, sizeof(sync_map_desc));
}

axres sync_map_ref_init(
	_in sync_map_desc	*smap,
	_in u32			index, // Member index to refernece
	_in_out sync_map_ref	*buf
){
	if (smap == nullptr){
		return AX_INV_ARG;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	sync_map_ref temp_smap = (sync_map_ref){
		.byte_index = index / sizeof(u64),
		.bit_index = index % (sizeof(u64) * 8),
		.map = smap->map,
		.size = smap->size,
	};
	memcpy(buf, &temp_smap, sizeof(sync_map_ref));

	return AX_SUCC;
}

