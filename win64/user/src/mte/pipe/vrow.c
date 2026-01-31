#include "mte/perf.h"

#include "vrow.h"

axres vrow_create(
	_in ir_context		*ir,
	_in_opt	u32		smap_i,
	_in_opt sync_map_desc	*smap,
	_out vrow_desc		**buf
){
	if (buf == nullptr){
		return AX_INV_BUF;
	}
	axres res = AX_SUCC;

	vrow_desc *vrow =
		axmalloc(sizeof(vrow_desc));

	/*
	 	Prefetch 256 bytes (max base buffer size)
	*/
	_mm_prefetch(vrow, _MM_HINT_T0);
	_mm_prefetch(offp(vrow, 64), _MM_HINT_T0);
	_mm_prefetch(offp(vrow, 128), _MM_HINT_T0);
	_mm_prefetch(offp(vrow, 192), _MM_HINT_T0);

	atomic_store(&vrow->states, VROW_STATE_EMPTY);

	if (smap != nullptr){
		res = sync_map_ref_init(smap, smap_i, &vrow->smap);
		axcheck_r(res, res, axfree(vrow));  // TODO: Change the return code
	}

	res = vrow_thread_init(vrow, ir, &vrow->thread);
	axcheck_r(res, res, axfree(vrow)); // TODO: Change the return code.

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

	while (atomic_load_explicit(&vrow->states, memory_order_acquire) != VROW_STATE_EMPTY){
		_mm_pause();
	}

	axfree(vrow);
}

bool vrow_bank_load(
	_in vrow_desc		*vrow,
	_in u8			bank_i,
	_in vrow_payload	payload
){
	while(vrow_is_filled(vrow, bank_i)){
		_mm_pause();
	}

	u8 *base_off = offp(vrow->base, bank_i * VROW_BANK_SIZE);
	simd_imax_store_512(base_off, &payload);

	vrow_fill_switch(vrow, bank_i);

	return false;
}

