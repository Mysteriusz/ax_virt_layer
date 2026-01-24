#include "mips32_ir.h"
#include "mips32_instr.h"
#include "tables/mips32_op.h"

axres mips32_raw_to_ir(
	_in mips32_mte_raw_instr	instr,
	_in_out ir_raw_instr		*buf
){
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	switch(_mips32_get_instr_type(instr)){
	case R:
		buf->reg[0] = init_ir_reg(_mips32_rd(instr));
		buf->reg[1] = init_ir_reg(_mips32_rs(instr));
		buf->reg[2] = init_ir_reg(_mips32_rt(instr));
		buf->opcode = MIPS32_IR_TABLE[_mips32_funct(instr)];
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

ir_raw_instr mips32_to_ir_call(
	_in mte_raw_instr	instr,	
	_in ir_context		*context
){
	ir_raw_instr buf = {0};
	axres res = mips32_raw_to_ir(instr.mips32, &buf);
	axcheck_r(res, (ir_raw_instr){0});

	return buf;
}

