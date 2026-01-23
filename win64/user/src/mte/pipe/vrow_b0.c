#include "vrow_b0.h"
#include "mte/perf.h"

void *vrow_b0_entry(
	struct vrow_bank_thread_stack		*stack
){
	_mm_prefetch(stack->vrow, _MM_HINT_T0);

	if (stack == nullptr){
		return nullptr;
	}

	vrow_b0_exec(stack->bank, stack->vrow);

	return nullptr;
}
void vrow_b0_exec(
	u8			bank,
	vrow_desc		*vrow
){
	__INL_PERF_INIT
	__INL_PERF_START

	// Parse payload
	struct vrow_b0_payload *b0
		= (struct vrow_b0_payload*)offp(vrow->base, VROW_BANK_SIZE * bank);

	// Parse IR structures
	ir_context* ir
		= (ir_context*)b0->control.context;
	struct ir_context_desc* ir_desc
		= (struct ir_context_desc*)ir->rule.data;

	// Translate using the IR context descriptor
	ir_raw_instr res = ir_desc->call.org_to_ir(b0->payload.instr, ir);
	if (res.opcode == IR_INVALID_OPCODE){
		return;
	}

	__INL_PERF_END
	__INL_PERF_LOG
	io_i64(res.opcode);

	return;
}

