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

	_mm_prefetch(buf->map, _MM_HINT_T0);

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
	_in u32			from_bit_index,	
	_in u32			to_bit_index,
	_in sync_map_desc	*smap,
	_out_opt u32		*index
){
	if (smap == nullptr){
		return false;
	}

	u32 from_bit = sync_map_ind2bit(from_bit_index);
	u32 from_quad = sync_map_ind2quad(from_bit_index);

	// Check bounds
	if (from_quad >= smap->size){
		return false;
	}

	// Mask for the first shift
	u64 from_mask = ((~0ULL) << from_bit);
	sync_map ptr = &smap->map[from_quad];

	// Try to mask out bit at first byte index
	u64 val = atomic_load_explicit(ptr, memory_order_acquire) & from_mask;

	u32 sigi = 0; // Signal index (relative to current from_quad)
	u32 desi = 0; // Desired index (smap bit index of sigi)

	if (val != from_mask){ // If val quad is not full
		// Calculate signal index on current quad with bitmask
		sigi = __builtin_ctzll(~val & from_mask);
		// Calculate smap desired index
		desi = sigi + (64 * from_quad);
		// Check bounds
		if (desi >= to_bit_index){
			return false;
		}
		// xor usign the relative bit index
		atomic_fetch_xor_explicit(
			ptr,
			(1ULL << sigi),
			memory_order_release);

		if (index != nullptr){
			*index = sigi + (64 * from_quad);
		}
		return true;
	}

	// Continue search
	while(++from_quad < smap->size){
		ptr++;
		val = atomic_load_explicit(ptr, memory_order_acquire);
		if (~val){
			sigi = __builtin_ctzll(~val);
			desi = sigi + (64 * from_quad);
			if (desi >= to_bit_index){
				return false;
			}

			atomic_fetch_xor_explicit(
				ptr,
				1ULL << sigi,
				memory_order_release);
			break;
		}
	}

	// Check if reached sync map limit
	if (from_quad == smap->size){
		return false;
	}

	if (index != nullptr){
		*index = sigi + (64 * from_quad);
	}

	return true;
}

