#if !defined(MTE_OPERAND_FIX_INT)
#define MTE_OPERAND_FIX_INT

#include "mte/ir/ir.h"

#include "compile_types.h"

/*
	Fill the association table
	between org (Guest) and tar (Host) operands
*/
void comp_fill_assoc(
	_in const ir_context 	*ir,
	_in ir_raw_instr 	*ir_instr,
	_in_out comp_reg_assoc	(*assoc)[0xff + IR_SPILL_LIMIT],
	_in_out comp_reg_state	(*state)[0xff + IR_SPILL_LIMIT]
);

/*
	Fix instruction by swapping it`s operands based
	on the associtation table filled by 'comp_fill_assoc'
*/
void comp_fix_instr(
	_in const ir_context 	*ir,
	_in_out ir_raw_instr 	*ir_instr,
	_in_out comp_reg_assoc	(*assoc)[0xff + IR_SPILL_LIMIT],
	_in_out comp_reg_state	(*state)[0xff + IR_SPILL_LIMIT]
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

*/
u32 comp_expand_instr(
	_in const ir_context 		*ir,
	_in const ir_raw_instr 		*restrict ir_instr,
	_in_out ir_raw_instr 		*restrict buf
);

/*
 	Clear association/state for the given liveness buffer
	at bit index of 'liveness_idx'
*/
void comp_flush_by_liveness(
	_in const ir_context 		*ir,
	_in const u8			liveness_idx,
	_in const comp_reg_liveness	(*liveness)[0xff],
	_in_out comp_reg_assoc		(*assoc)[0xff + IR_SPILL_LIMIT],
	_in_out comp_reg_state		(*state)[0xff + IR_SPILL_LIMIT]
);

#endif // !defined(MTE_OPERAND_FIX_INT)

