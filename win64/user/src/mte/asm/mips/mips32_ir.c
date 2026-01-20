#include "mips32_ir.h"
#include "tables/mips32_op_table.h"
#include "tables/mips32_reg_table.h"

axres mips32_raw_to_ir(
	_in mips32_mte_raw_instr	instr,
	_in const ir_rule		*rule,
	_in_out ir_raw_instr		*buf
){
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	switch(_mips32_check_type(instr)){
	case R:
		buf->reg[0] = init_ir_reg(mips32_rd(instr));
		buf->reg[1] = init_ir_reg(mips32_rs(instr));
		buf->reg[2] = init_ir_reg(mips32_rt(instr));
		buf->opcode = ((u32*)rule->data)[0];
		buf->reg_used = 3;
		break;
	case I:
		break;
	case J:
		break;
	default:
		return AX_MTE_INV_INSTR;
	}

	return AX_SUCC;
}

