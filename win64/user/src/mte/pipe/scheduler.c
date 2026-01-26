#include "scheduler.h"

axres sched_create(
	_in ir_context		*ir,
	_in vrow_desc		*vrow,
	_out sched_context	**buf
){
	if (vrow == nullptr){
		return AX_INV_ARG;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	// Allocate action for the vrow
	u8 vrow_act = vrow_alloc_action(vrow);
	if (vrow_act == 0){
		return AX_IR_BLOCKED; // TODO: Change the error code
	}

	// Create temporary IR context
	sched_context temp_sched = (sched_context){
		.ir = ir,
		.vrow = vrow,
		.vrow_act = vrow_act,
	};
	sched_context *sched =
		axmalloc(sizeof(sched_context));
	memcpy(sched, &temp_sched, sizeof(sched_context));

	/*
	 	Initialize thread of the scheduler
	*/
	pthread_attr_t attr = {0};
	pthread_attr_init(&attr);
	pthread_create(
		&sched->thread,
		&attr,
		(void *(*)(void*))sched_main,
		sched);

	*buf = sched;

	return AX_SUCC;
}

void *sched_main(
	sched_context 		*sched
){
	while(!vrow_is_closed(sched->vrow)){
		_mm_pause();
	}
	vrow_action_switch(sched->vrow, sched->vrow_act);
	return nullptr;
}

_inline_avert void sched_delete(
	_in sched_context	*sched
){
	// TODO: Flush the scheduler queue

	if (sched != nullptr){
		axfree(sched);
	}
}

void sched_to_next(
	_in sched_context 	*sched,
	_in u8 			bank_i
){
	// Try to schedule to next bank without queue
	if (vrow_bank_move(sched->vrow, bank_i, bank_i + 1)){
		// TEMPORARY TO ALLOW DELETE!
		vrow_fill_switch(sched->vrow, (bank_i + 1));
		io_str(u"No queue");
		return;
	}
	vrow_fill_switch(sched->vrow, bank_i);
	io_str(u"To queue");
	return;
}

