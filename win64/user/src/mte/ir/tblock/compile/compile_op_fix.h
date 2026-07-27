#if !defined(MTE_OPERAND_FIX_INT)
#define MTE_OPERAND_FIX_INT

#include "mte/ir/ir.h"

#include "compile_types.h"

/*
	Fill assoc between org (Guest) and tar (Host) registers
*/
void comp_fill_assoc(
	_in const ir_context 	*ir,
	_in ir_raw_instr 	*ir_instr,
	_in_out comp_reg_assoc	(*assoc)[0xff + IR_SPILL_LIMIT],
	_in_out comp_reg_state	(*state)[0xff + IR_SPILL_LIMIT]
);

void comp_fix_instr(
	_in const ir_context 	*ir,
	_in_out ir_raw_instr 	*ir_instr,
	_in_out comp_reg_assoc	(*assoc)[0xff + IR_SPILL_LIMIT],
	_in_out comp_reg_state	(*state)[0xff + IR_SPILL_LIMIT]
);

void comp_flush_by_liveness(
	_in const ir_context 		*ir,
	_in const u8			liveness_idx,
	_in const comp_reg_liveness	(*liveness)[0xff],
	_in_out comp_reg_assoc		(*assoc)[0xff + IR_SPILL_LIMIT],
	_in_out comp_reg_state		(*state)[0xff + IR_SPILL_LIMIT]
);

#endif // !defined(MTE_OPERAND_FIX_INT)

