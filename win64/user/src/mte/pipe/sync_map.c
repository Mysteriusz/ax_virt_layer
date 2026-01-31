#include "sync_map.h"

axres sync_map_init(
	_in u32 		size,
	_in_out sync_map_desc	*buf
){
	// Has to be a mulitply of 64
	if (size % 64 != 0){
		return AX_INV_ARG;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	sync_map_desc temp_smap = (sync_map_desc){
		.size = size / 64,
		.map = axmalloc(size / 8),
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
		.quad_index = sync_map_ind2quad(index),
		.byte_index = sync_map_ind2byte(index),
		.bit_index = sync_map_ind2bit(index),
		.map = smap->map,
		.size = smap->size,
	};
	memcpy(buf, &temp_smap, sizeof(sync_map_ref));

	return AX_SUCC;
}

bool sync_map_sig_first(
	_in u32			bit_index,
	_in sync_map_desc	*smap
){
	if (smap == nullptr){
		return false;
	}

	u32 bit = sync_map_ind2bit(bit_index);
	u32 quad = sync_map_ind2quad(bit_index);

	// Check bounds
	if (quad >= smap->size){
		return false;
	}

	// Mask for the first shift
	u64 mask = ((~0ULL) << bit);
	sync_map ptr = &smap->map[quad];

	// Try to mask out bit at first byte index
	u64 val = atomic_load_explicit(ptr, memory_order_acquire) & mask;
	if (val != mask){ // If previous & did not returned only 1 bits
		atomic_fetch_xor_explicit(ptr, (1ULL << (__builtin_ctzll(~val & mask))), memory_order_release);
		return true;
	}

	// Continue search
	while(++quad < smap->size){
		ptr++;
		val = atomic_load_explicit(ptr, memory_order_acquire);
		if (~val){
			atomic_fetch_xor(ptr, 1ULL << (__builtin_ctzll(~val)));
		}
	}

	// Check if reached sync map limit
	if (quad == smap->size){
		return false;
	}

	return true;
}

