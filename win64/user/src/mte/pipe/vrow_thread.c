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

	// Tell vrow that thread is sitting at bank 0 
	// (Before thread creation to avoid heap corruptions if thread doesnt exectue fast enough)
	vrow_active_switch(vrow, 0);

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
	sync_map_ref *const 	smap = &vrow->smap;

	// Assert vrow present
	asrt(vrow != nullptr);
	// Assert sync map present (Required for vrow state signaling)
	asrt(smap != nullptr);
	// Assert thread active on start
	asrt(vrow_is_active(vrow, 0));

	// Preload conversion data
	org_to_ir_call b0_func = 
		ir_rule_to_context(&stack->ir->rule)->call.org_to_ir;

	// Main thread loop
	while(!vrow_is_closed(vrow)){
		if (!vrow_is_filled(vrow, 0)){
			_mm_pause();
			continue;
		}

		__INL_PERF_INIT
		__INL_PERF_START

		// Signal thread business
		sync_map_sigon(smap);

		/*
		 	Pipelined processing
		*/

		// Process data at bank 0
		if (!vrow_bank_0_proc(vrow, b0_func)){
			// Force thread exit on fail
			// TODO: backout and reset the thread instead of breaking
			sync_map_sigoff(smap);
			break;
		}

		// Tell vrow that thread is now at bank 1
		vrow_active_switch(vrow, 1);
		vrow_active_switch(vrow, 0);

		// Signal thread emptiness
		sync_map_sigoff(smap);

		__INL_PERF_END
		__INL_PERF_LOG
	}
	_vrow_force_flush(vrow);
	return nullptr;
}

