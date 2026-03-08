#include "ir_mapping.h"

bool ir_create_instr_mapping(
	_in struct ir_context_desc	*desc,
	/*
	 	Instruction translated via [ir_context_desc->call.org_to_ir]
	*/
	_in ir_raw_instr 		instr,
	_out struct ir_instr_map	*buf
){
	if (__builtin_expect(desc == nullptr, false)){
		return false;
	}
	if (__builtin_expect(buf == nullptr, false)){
		return false;
	}
	if (__builtin_expect(instr.opcode == IR_INVALID_OPCODE, false)){
		return false;
	}
	
	for (u8 i = 0; i < instr.reg_used; i++){
		ir_operand *op = &instr.regs[i];

		// org register role
		enum cpu_reg_role org_role =
			desc->call.org_reg_to_role(op->value);

		switch(op->id){
		case IR_OP_REG:
			// tar register id
			u8 reg = desc->call.tar_role_to_reg(org_role);
			buf->map[i] = op->value << 8 | reg;
			break;
		case IR_OP_IMM:
			break;
		case IR_OP_MEM:
			break;
		default:
			continue;
		}
	}

	buf->count = instr.reg_used;

	return true;
}

