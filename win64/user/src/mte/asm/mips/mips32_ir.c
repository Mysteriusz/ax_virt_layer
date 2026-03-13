#include "mips32_ir.h"
#include "mips32_instr.h"
#include "mte/perf.h"

axres mips32_raw_to_ir(
	_in mips32_mte_raw_instr	instr,
	_in_out ir_raw_instr		*buf
){
	if (__builtin_expect(buf == nullptr, false)){
		return AX_INV_BUF;
	}

	switch(_mips32_get_instr_type(instr)){
	case R:
		buf->opcode = MIPS32_TO_IR_TABLE[_mips32_funct(instr)];
		buf->set.ops[0] = (ir_operand){
			.id = IR_OP_REG,
			.value = _mips32_rd(instr)
		};
		buf->set.ops[1] = (ir_operand){
			.id = IR_OP_REG,
			.value = _mips32_rs(instr)
		};
		buf->set.ops[2] = (ir_operand){
			.id = IR_OP_REG,
			.value = _mips32_rt(instr)
		};
		buf->set.op_count = 3;
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
	_in ir_context		*ctx,
	_out u8			*len
){
	if (__builtin_expect(len == nullptr, false)){
		return (ir_raw_instr){0};
	}

	ir_raw_instr buf = {0};
	axres res = mips32_raw_to_ir(instr.payload[0], &buf);
	axcheck_r(res, (ir_raw_instr){0});

	*len = 4; // Always 4 bytes

	return buf;
}

ir_operand_set mips32_reg_fetch_call(
	_in mte_raw_instr	instr,	
	_in ir_context		*ctx,
	_out u8			*len
){
	if (__builtin_expect(len == nullptr, false)){
		return (ir_operand_set){0};
	}
	*len = 4;

	ir_operand_set buf = {0};
	mips32_mte_raw_instr mips32 = instr.payload[0];

	switch(_mips32_get_instr_type(mips32)){
	/*
	 	rd = rs + rt
	*/
	case R:
		buf.ret = (ir_operand){
			.id = IR_OP_REG,
			.value = _mips32_rd(mips32)
		};
		buf.ops[0] = (ir_operand){
			.id = IR_OP_REG,
			.value = _mips32_rs(mips32)
		};
		buf.ops[1] = (ir_operand){
			.id = IR_OP_REG,
			.value = _mips32_rt(mips32)
		};
		buf.op_count = 2;
		break;
	case I:
		break;
	case J:
		break;
	default:
		return (ir_operand_set){0};
	}
	return buf;
}


