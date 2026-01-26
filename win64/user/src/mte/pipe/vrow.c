#include "vrow.h"

axres vrow_create(
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
void vrow_delete(
	_in vrow_desc		*vrow
){
	if (vrow == nullptr){
		return;
	}

	vrow_close(vrow);

	while (atomic_load(&vrow->states) != VROW_STATE_EMPTY
	|| atomic_load(&vrow->close) != VROW_CLOSE_EMPTY){
		io_i64(atomic_load(&vrow->states));
		io_i64(atomic_load(&vrow->close));
		_mm_pause();
	}

	axfree(vrow);
}

volatile bool vrow_load(
	_in vrow_desc		*vrow,
	_in vrow_payload	payload
){
	if (vrow == nullptr){
		return false;
	}
	
	u8 i = vrow_alloc_thread(vrow);

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
	while (vrow_is_filled(vrow, i)){
		_mm_pause();
	}

	// Set the bank state to filled
	vrow_fill_switch(vrow, i);

	volatile u8 bank_off = VROW_BANK_SIZE * i;
	// 64 bytes payload to bank copy
	simd_imax_store_512(offp(vrow->base, bank_off), &payload);

	// Refresh new base with cache
	_mm_prefetch(vrow->base + (64 * i), _MM_HINT_T0);

	return true;
}

volatile bool vrow_bank_move(
	_in vrow_desc		*vrow,
	_in u8			from, // From bank index
	_in u8			to // To bank index
){
	if (vrow == nullptr){
		return false;
	}

	if (vrow_is_filled(vrow, to)){
		return false;
	}

	vrow_fill_switch(vrow, to);

	// Copy from one bank to another
	u8 *from_off = offp(vrow->base, VROW_BANK_SIZE * from);
	u8 *to_off = offp(vrow->base, VROW_BANK_SIZE * to);
	simd_imax_store_512(from_off, to_off);

	vrow_fill_switch(vrow, from);

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
	if(!vrow_is_filled(vrow, i)){
		return false;
	}

	// Switch bank state to not-filled
	vrow_fill_switch(vrow, i);
	
	u8 bank_off = VROW_BANK_SIZE * i;
	// cleanup 64 bytes from bank
	simd_imax_zero_512(offp(vrow->base, bank_off));

	return true;
}

