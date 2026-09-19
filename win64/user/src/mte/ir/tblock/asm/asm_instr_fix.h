#if !defined(MTE_OPERAND_FIX_INT)
#define MTE_OPERAND_FIX_INT

#include "mte/ir/ir.h"

#include "asm_types.h"

/*
	Fill the association table
	between org (Guest) and tar (Host) operands
*/
void asm_fill_assoc(
	_in const ir_context 	*ir,
	_in ir_raw_instr 	*ir_instr,
	_in_out asm_reg_assoc	(*assoc)[IR_SPILL_REG_LIMIT],
	_in_out asm_reg_state	(*state)[IR_SPILL_REG_LIMIT]
);

/*
	Fix instruction by swapping it`s operands based
	on the associtation table filled by 'asm_fill_assoc'
*/
void asm_fix_instr(
	_in const ir_context 	*ir,
	_in_out ir_raw_instr 	*ir_instr,
	_in_out asm_reg_assoc	(*assoc)[IR_SPILL_REG_LIMIT]
);

/*
 	Expand instruction into multiple subsequent ones
	if it`s required

	When instruction is to be translated to an ISA
	which requires dest/src0 operand and the current
	instruction is not compliant then expand it 
	to 2 subsequent instructions

	Example:
		IR:

		- add r0, r1, r2

		Expanded:

		- mov r0, r1
		- add r0, r2

	Returns the amount of IR instructions
	written to buf
*/
u32 asm_expand_instr(
	_in const ir_context 		*ir,
	_in const ir_raw_instr 		*restrict ir_instr,
	_in const u32			buf_len,
	_in_out ir_raw_instr 		*restrict buf
);

/*
 	Clear association/state for the given liveness buffer
	at bit index of 'liveness_idx'
*/
void asm_flush_by_liveness(
	_in const ir_context 		*ir,
	_in const u8			liveness_idx,
	_in const asm_reg_liveness	(*liveness)[IR_REG_LIMIT],
	_in_out asm_reg_assoc		(*assoc)[IR_SPILL_REG_LIMIT],
	_in_out asm_reg_state		(*state)[IR_SPILL_REG_LIMIT]
);

#endif // !defined(MTE_OPERAND_FIX_INT)

