#include "mte/perf.h"

#include "vrow.h"
#include "vrow_bank.h"

axres vrow_thread_init(
	_in vrow_desc		*vrow,
	_in ir_context		*ir,
	_in_out vrow_thread	*th
){
	if (th == nullptr){
		return 0;
	}

	th->stack.vrow = vrow;
	th->stack.curr_bank = 0;
	th->stack.ir = ir;

	i32 res = 0;
	pthread_attr_t attr = {0};
	res = pthread_attr_init(&attr);
	if (res != 0){
		return AX_UNK_ERR; // TODO: Different code
	}

	res = pthread_create(
		&th->pthread,
		&attr,
		(void* (*)(void*))vrow_thread_main,
		&th->stack);
	if (res != 0){
		return AX_UNK_ERR; // TODO: Different code
	}

	return AX_SUCC;
}

void *vrow_thread_main(
	struct _vrow_thread_stack *stack
){
	/*
	 	Vrow constant references
	*/
	vrow_desc *const 	vrow = stack->vrow;
	sync_map_desc *const 	smap = &vrow->smap;

	u8 i = smap->index / sizeof(u64); // bitmap data offset index
	u8 bi = smap->index % (sizeof(u64) * 8); // bit index

	// Preload conversion data
	org_to_ir_call b0_func = ir_rule_to_context(&stack->ir->rule)->call.org_to_ir;

	// Main thread loop
	while(!vrow_is_closed(vrow)){
		if (!vrow_is_filled(vrow, 0)){
			_mm_pause();
			continue;
		}

		__INL_PERF_INIT
		__INL_PERF_START

		// Signal thread business
		sync_map_sigi(smap->map, i, bi);

		/*
		 	Pipelined processing
		*/

		// Process data at bank 0
		if (!vrow_bank_0_proc(vrow, b0_func)){
			sync_map_sigi(smap->map, i, bi);
			break;
		}

		// Signal thread emptiness
		sync_map_sigi(smap->map, i, bi);

		__INL_PERF_END
		__INL_PERF_LOG
		//_sync_map_sigi(smap);
		//vrow_fill_switch(vrow, 3);
	}
	return nullptr;
}

axres vrow_create(
	_in ir_context		*ir,
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
		vrow_link_sync(vrow, *smap);
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

	while (atomic_load(&vrow->states) != VROW_STATE_EMPTY){
		//io_i64(atomic_load(&vrow->states));
		//io_i64(atomic_load(&vrow->closed));
		_mm_pause();
	}

	axfree(vrow);
}

bool vrow_link_sync(
	_in vrow_desc 		*vrow,
	_in sync_map_desc	map
){
	if (vrow == nullptr){
		return false;
	}
	memcpy(&vrow->smap, &map, sizeof(sync_map_desc));
	return true;
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

