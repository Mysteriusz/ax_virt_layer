#if !defined(MTE_MIPS32_IR_INT)
#define MTE_MIPS32_IR_INT

#include "mte/ir/ir.h"
#include "mips32.h"

axres mips32_raw_to_ir_call(
	_in mips32_mte_raw_instr	instr,
	_in_out ir_raw_instr		*buf // Initialize before use by init_ir_raw_instr
);

/*
 	Interface call for context based translation
*/
ir_raw_instr mips32_to_ir_call(
	_in mte_raw_instr	instr,	
	_in ir_context		*ctx,
	_out u8			*len
);
mte_raw_instr mips32_to_tar_call(
	_in mte_raw_instr	instr,
	_in ir_context		*ctx,
	_out u8			*len
);

ir_operand_set mips32_reg_fetch_call(
	_in mte_raw_instr	instr,	
	_in ir_context		*ctx,
	_out u8			*len
);

#endif // !defined(MTE_MIPS32_IR_INT)

