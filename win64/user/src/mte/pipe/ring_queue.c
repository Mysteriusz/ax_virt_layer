#include "ring_queue.h"

axres init_rqueue(
	_in rqueue_desc		*rqueue
){
	if (rqueue == nullptr){
		return AX_INV_BUF;
	}

	u8 *base = rqueue->base;

	/*
	 	Prefetch 256 bytes (max base buffer size)
	*/
	_mm_prefetch(base, _MM_HINT_T0);
	_mm_prefetch(base + 64, _MM_HINT_T0);
	_mm_prefetch(base + 128, _MM_HINT_T0);
	_mm_prefetch(base + 192, _MM_HINT_T0);

	(void)rqueue_region_push(nullptr, 0xff, (rqueue_payload){0});

	return AX_SUCC;
}

volatile bool rqueue_region_push(
	_in rqueue_desc		*rqueue,
	_in u8			region_i,
	_in rqueue_payload	payload
){
	if (rqueue == nullptr){
		return false;
	}

	u8 reg_i = region_i & 0x3;
	if (rqueue->r_states[reg_i]){
		return false;
	}

	volatile u8 reg_off = RQUEUE_REGION_SIZE * reg_i;
	// 64 bytes move
	simd_imax_store_512(offp(rqueue->base, reg_off), &payload);

	return true;
}

