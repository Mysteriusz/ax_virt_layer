#include "vrow_b0.h"
#include "mte/perf.h"

void *vrow_b0_entry(
	struct vrow_bank_thread_stack		*stack
){
	_mm_prefetch(stack->vrow, _MM_HINT_T0);

	if (stack == nullptr){
		return nullptr;
	}

	// Parse payload
	volatile struct vrow_b0_payload *const b0 
		= (struct vrow_b0_payload*)offp(stack->vrow->base, VROW_BANK_SIZE * stack->bank);
	// Parse IR context
	volatile ir_context *const ir
		= (ir_context*)b0->control.context;
	// Parse IR context descriptor
	volatile struct ir_context_desc *const ir_desc
		= (struct ir_context_desc*)ir->rule.data;

	_Atomic u8 *states = &stack->vrow->states;
	while(!vrow_is_locked(states, stack->bank)){
 		// Vrow is being deleted
		if (vrow_is_closed(states)){
			vrow_lock_switch(states, 0);
			vrow_fill_switch(states, 0);
			return nullptr;
		}
		if (!vrow_is_filled(states, stack->bank)){
			_mm_pause();
			continue;
		}

		vrow_b0_exec(
			stack->vrow,
			ir,
			ir_desc,
			b0
		);
	}

	return nullptr;
}
void vrow_b0_exec(
	vrow_desc				*vrow,
	volatile ir_context *const 		ir,
	volatile struct ir_context_desc *const 	ir_desc,
	volatile struct vrow_b0_payload *const 	b0
){
	__INL_PERF_INIT
	__INL_PERF_START

	// Translate using the IR context descriptor
	ir_raw_instr res =
		ir_desc->call.org_to_ir(b0->payload.instr, (ir_context*)ir);

	// Invlidate result
	if (res.opcode == IR_INVALID_OPCODE){
		return;
	}

	__INL_PERF_END
	__INL_PERF_LOG

	vrow_fill_switch(&vrow->states, 0);
	return;
}

