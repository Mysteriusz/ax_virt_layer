#include "mte/asm/mips/mips32_ir.h"
#include "mte/asm/mips/mips32_opcode_table.h"
#include "mte/asm/mips/mips32_reg_table.h"

axres mips32_raw_to_ir(
	_in mips32_mte_raw_instr	instr,
	_in_out ir_raw_instr		*buf
){
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	switch(mips32_check_type(instr)){
	case R:
		buf->opcode = _mips32_opcode_funct_table[mips32_funct(instr)].ir_code;
		buf->reg[0] = mips32_rd(instr) * sizeof(reg32);
		buf->reg[1] = mips32_rs(instr) * sizeof(reg32);
		buf->reg[2] = mips32_rt(instr) * sizeof(reg32);
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

