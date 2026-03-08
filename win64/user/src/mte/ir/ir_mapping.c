#include "ir_mapping.h"

bool ir_create_instr_mapping(
	_in struct ir_context_desc	*desc,
	/*
	 	Instruction translated via [ir_context_desc->call.org_to_ir]
	*/
	_in ir_raw_instr 		instr,
	_out struct ir_instr_map	*map
){
	if (__builtin_expect(desc == nullptr, false)){
		return false;
	}
	if (__builtin_expect(map == nullptr, false)){
		return false;
	}
	if (__builtin_expect(instr.opcode == IR_INVALID_OPCODE, false)){
		return false;
	}
	
	for (u8 i = 0; i < instr.reg_used; i++){
		ir_operand *op = &instr.regs[i];
		switch(op->id){
		case IR_OP_REG:
			break;
		default:
			return false;
		}
	}

	map->count = instr.reg_used;

	return true;
}

