#include "vrow.h"

axres init_vrow(
	_in vrow_desc		*vrow
){
	if (vrow == nullptr){
		return AX_INV_BUF;
	}

	u8 *base = vrow->base;

	/*
	 	Prefetch 256 bytes (max base buffer size)
	*/
	_mm_prefetch(base, _MM_HINT_T0);
	_mm_prefetch(base + 64, _MM_HINT_T0);
	_mm_prefetch(base + 128, _MM_HINT_T0);
	_mm_prefetch(base + 192, _MM_HINT_T0);

	(void)vrow_bank_load(nullptr, 0xff, (vrow_payload){0});

	return AX_SUCC;
}

volatile bool vrow_load(
	_in vrow_desc		*vrow,
	_in vrow_payload	payload
){
	if (vrow == nullptr){
		return false;
	}

	u8 xor = vrow->states ^ 0xff;
	if (!xor){
		return false;
	}

	u8 i = __builtin_ctzl(xor) >> 1;
	u8 bank_off = VROW_BANK_SIZE * i;
	// 64 bytes payload to bank copy
	simd_imax_store_512(offp(vrow->base, bank_off), &payload);

	vrow->states |= (0x3 << (i << 1));

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
	if (vrow->states & (0x3 << i)){
		return false;
	}

	volatile u8 bank_off = VROW_BANK_SIZE * i;
	// 64 bytes payload to bank copy
	simd_imax_store_512(offp(vrow->base, bank_off), &payload);

	vrow->states |= (3 << i);

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
	if (!(vrow->states & (0x3 << (i << 1)))){
		return false;
	}
	
	u8 bank_off = VROW_BANK_SIZE * i;
	// cleanup 64 bytes from bank
	simd_imax_zero_512(offp(vrow->base, bank_off));

	vrow->states &= ~(0x3 << (i << 1));

	return true;
}

