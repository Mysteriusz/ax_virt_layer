#include <ax_type.h>

#include "mips32_ir.h"
#include "mips32_opcode.h"
#include "mips32_instr.h"

axres mips32_raw_to_ir(
	_in mips32_mte_raw_instr	mips32_instr,
	_in_out ir_raw_instr		*buf
){
	if (__builtin_expect(buf == nullptr, false)){
		return AX_INV_BUF;
	}

	switch(_mips32_get_instr_type(mips32_instr)){
	case R:
		buf->opcode = MIPS32_TO_IR_TABLE[_mips32_funct(mips32_instr)];
		buf->set.ops[0] = (ir_operand){
			.kind = IR_OP_REG,
			.id = _mips32_rd(mips32_instr)
		};
		buf->set.ops[1] = (ir_operand){
			.kind = IR_OP_REG,
			.id = _mips32_rs(mips32_instr)
		};
		buf->set.ops[2] = (ir_operand){
			.kind = IR_OP_REG,
			.id = _mips32_rt(mips32_instr)
		};
		buf->set.ops_count = 3;
		break;
	case I:
		asrt(0, io_afstr(ANSI("MIPS32 I-type instructions not implemented. %lli"), AX_NOT_IMP));
		break;
	case J:
		asrt(0, io_afstr(ANSI("MIPS32 J-type instructions not implemented. %lli"), AX_NOT_IMP));
		break;
	default:
		return AX_MTE_INV_INSTR;
	}

	return AX_SUCC;
}

ir_raw_instr mips32_to_ir(
	_in mte_raw_instr	*raw_instr,
	_in ir_context		*ctx,
	_out u8			*len
){
	if (__builtin_expect(raw_instr == nullptr, false)){
		return (ir_raw_instr){0};
	}
	if (__builtin_expect(ctx == nullptr, false)){
		return (ir_raw_instr){0};
	}
	if (__builtin_expect(len == nullptr, false)){
		return (ir_raw_instr){0};
	}

	ir_raw_instr buf = {0};
	axres res = mips32_raw_to_ir(*(u32*)raw_instr->payload, &buf);
	axcheck_r(res, (ir_raw_instr){0});

	*len = 4; // Always 4 bytes

	return buf;
}

ir_operand_set mips32_reg_fetch(
	_in mte_raw_instr	*raw_instr,
	_in ir_context		*ctx,
	_out u8			*len
){
	unref(ctx);
	if (__builtin_expect(len == nullptr, false)){
		return (ir_operand_set){0};
	}

	*len = 4;

	ir_operand_set buf = {0};
	mips32_mte_raw_instr mips32 = *(u32*)raw_instr->payload;

	switch(_mips32_get_instr_type(mips32)){
	/*
	 	rd = opcode rs, rt
	*/
	case R:
		if (_mips32_funct(mips32) == 0){
			break;
		}

		buf.ops[0] = (ir_operand){ // rd
			.kind = IR_OP_REG,
			.id = _mips32_rd(mips32)
		};
		buf.ops[1] = (ir_operand){ // rs
			.kind = IR_OP_REG,
			.id = _mips32_rs(mips32)
		};
		buf.ops[2] = (ir_operand){ // rt
			.kind = IR_OP_REG,
			.id = _mips32_rt(mips32)
		};
		buf.ops_count = 3;
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


