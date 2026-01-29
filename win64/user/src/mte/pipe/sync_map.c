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
		.index = 0,
		.size = size,
		.map = axmalloc(size * sizeof(u64)),
		.owner = true
	};
	memcpy(buf, &temp_smap, sizeof(buf->meta));

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

axres sync_map_copy(
	_in sync_map_desc	*smap,
	_in_out sync_map_desc	*buf
){
	if (smap == nullptr){
		return AX_INV_ARG;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	sync_map_desc temp_smap = (sync_map_desc){
		.index = smap->index,
		.map = smap->map,
		.size = smap->size,
		.owner = false
	};
	memcpy(buf, &temp_smap, sizeof(sync_map_desc));

	return AX_SUCC;
}

