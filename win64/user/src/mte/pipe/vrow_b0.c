#include "mte/perf.h"

#include "vrow_b0.h"
#include "scheduler.h"

void *vrow_b0_main(
	struct vrow_bank_thread_stack		*stack
){
	if (stack == nullptr){
		return nullptr;
	}

	vrow_desc *vrow = stack->sched->vrow;
	_mm_prefetch(vrow, _MM_HINT_T0);

	/*
	 	Lock and wait for payload or vrow closure
	*/
	while(!vrow_is_filled(vrow, stack->bank)){
		_mm_pause();
		if (vrow_is_locked(vrow, stack->bank) 
			|| vrow_is_closed(vrow)){
			goto exit;
		}
	}

	// Parse payload
	volatile struct vrow_b0_payload *const b0 
		= (struct vrow_b0_payload*)offp(vrow->base, VROW_BANK_SIZE * stack->bank);
	// Parse IR context
	volatile ir_context *const ir
		= (ir_context*)b0->control.context;
	// Parse IR context descriptor
	volatile struct ir_context_desc *const ir_desc
		= (struct ir_context_desc*)ir->rule.data;

	// Enter the thread loop
	while(!vrow_is_locked(vrow, stack->bank) 
		&& !vrow_is_closed(vrow)){
 		// Vrow is being deleted
		if (!vrow_is_filled(vrow, stack->bank)){
			_mm_pause();
			continue;
		}

		vrow_b0_exec(
			vrow,
			stack->sched,
			ir,
			ir_desc,
			b0
		);
	}

exit:
	if (vrow_is_closed(vrow)){
		if (vrow_is_filled(vrow, stack->bank)){
			vrow_fill_switch(vrow, stack->bank);
		}
		vrow_lock_switch(vrow, stack->bank);
	}

	return nullptr;
}
void vrow_b0_exec(
	vrow_desc *const			vrow,
	sched_context *const 			sched,
	volatile ir_context *const 		ir,
	volatile struct ir_context_desc *const 	ir_desc,
	volatile struct vrow_b0_payload *const 	b0
){
	/*__INL_PERF_INIT
	__INL_PERF_START*/

	// Translate using the IR context descriptor
	ir_raw_instr res =
		ir_desc->call.org_to_ir(b0->payload.instr, (ir_context*)ir);

	// Invlidate result
	if (res.opcode == IR_INVALID_OPCODE){
		return;
	}

	// Move to next bank or to queue
	sched_to_next(sched, 0);
}

