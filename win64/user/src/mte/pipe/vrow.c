#include "vrow.h"

axres init_vrow(
	_out vrow_desc		**buf
){
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	vrow_desc *vrow = axmalloc(sizeof(vrow_desc));
	u8 *base = vrow->base;

	/*
	 	Prefetch 256 bytes (max base buffer size)
	*/
	_mm_prefetch(base, _MM_HINT_T0);
	_mm_prefetch(base + 64, _MM_HINT_T0);
	_mm_prefetch(base + 128, _MM_HINT_T0);
	_mm_prefetch(base + 192, _MM_HINT_T0);

	atomic_store(&vrow->states, 0b10101010); // All threads are initialy inactive
	(void)vrow_bank_load(nullptr, 0xff, (vrow_payload){0});

	*buf = vrow;

	return AX_SUCC;
}

volatile bool vrow_load(
	_in vrow_desc		*vrow,
	_in vrow_payload	payload
){
	if (vrow == nullptr){
		return false;
	}

	u8 any = vrow_is_any(&vrow->states);
	while(!any){
		any = vrow_is_any(&vrow->states);
		_mm_pause();
	}
	u8 i = __builtin_ctzl(any) >> 1;

	// Update state to filled
	vrow_fill_switch(&vrow->states, i);

	u8 bank_off = VROW_BANK_SIZE * i;
	// 64 bytes payload to bank copy
	simd_imax_store_512(offp(vrow->base, bank_off), &payload);

	_mm_prefetch(vrow->base + (64 * i), _MM_HINT_T0);

	return true;
}

volatile bool vrow_bank_load(
	_in vrow_desc		*vrow,
	_in u8			bank_i,
	_in vrow_payload	payload
){
	if (vrow == nullptr){
		return false;
	}

	u8 i = bank_i & 0x3;
	while (vrow_is_filled(&vrow->states, i)){
		_mm_pause();
	}
	// Set the bank state to filled
	vrow_fill_switch(&vrow->states, i);

	volatile u8 bank_off = VROW_BANK_SIZE * i;
	// 64 bytes payload to bank copy
	simd_imax_store_512(offp(vrow->base, bank_off), &payload);

	// Refresh new base with cache
	_mm_prefetch(vrow->base + (64 * i), _MM_HINT_T0);

	return true;
}

volatile bool vrow_bank_unload(
	_in vrow_desc		*vrow,
	_in u8			bank_i
){
	if (vrow == nullptr){
		return false;
	}

	u8 i = bank_i & 0x3;
	if (!vrow_is_filled(&vrow->states, i)){
		return false;
	}
	// Switch bank state to not-filled
	vrow_fill_switch(&vrow->states, i);
	
	u8 bank_off = VROW_BANK_SIZE * i;
	// cleanup 64 bytes from bank
	simd_imax_zero_512(offp(vrow->base, bank_off));


	return true;
}

