#include "vrow_bank.h"

bool vrow_bank_0_proc(
	_in vrow_desc		*vrow,
	_in org_to_ir_call	func
){
	// Get the payload
	struct vrow_b0_payload *b0 =
		(struct vrow_b0_payload*)offp(vrow->base, VROW_B0_OFFSET);

	// Execute bank processing function
	b0->payload.result = 
		func(b0->payload.instr, vrow->thread.stack.ir);

	// Check if execution failed
	if (b0->payload.result.opcode == IR_INVALID_OPCODE){
		return false;
	}

	return true;
}

