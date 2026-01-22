#include "mte/ir/ir.h"

#include "mips32.h"

// Maximum mnemonic length for currently supported mips32 instructions
#define MIPS32_MNEM_LEN_MAX 8
struct mips32_op_info{
	const u64		mnem_u64;
	const enum mips32_type 	type;
	const u8		opcode;
	ir_rule 		ir_rule;
};
struct mips32_reg_info{
	const u64		name_u64;
	const enum mips32_reg 	val;
	ir_rule 		ir_rule;
};

axres mips32_raw_to_ir(
	_in mips32_mte_raw_instr	instr,
	_in const ir_rule		*rule,
	_in_out ir_raw_instr		*buf // Initialize before use by init_ir_raw_instr
);

/*
 	Interface call for context based translation
*/
ir_raw_instr mips32_to_ir_call(
	_in mips32_mte_raw_instr	instr,	
	_in ir_context			*context
);

