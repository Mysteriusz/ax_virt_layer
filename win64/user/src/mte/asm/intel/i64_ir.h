#include "mte/ir/ir.h"

#include "intel/instr/i64_instr.h"
#include "intel/instr/i64_operand.h"
#include "intel/instr/i64_opcode.h"

ir_raw_instr i64_raw_to_ir(
	_in mte_raw_instr 	instr,
	_in ir_context 		*ir,
	_out u8			*len // Original instruction length (in bytes)
);

mte_raw_instr i64_ir_to_raw(
	_in ir_raw_instr 	instr,
	_in ir_context 		*ir,
	_out u8			*len // Target instruction length (in bytes)
);

extern enum i64_opcode I64_IR_GROUP_TO_GENERIC[][16];
extern enum i64_opcode I64_IR_GENERIC_TO_SPECIFIC[][16];

// Form based on first and second operands
extern enum i64_opcode_form I64_IR_FORM_MAP[4][4];

extern enum i64_operand_id I64_IR_TO_ID[][16];

/*
	Resolve I64 instruction form based on the first 2 IR operands.
*/
_inline_force enum i64_opcode_form _i64_ir_opcode_form_res(
	_in ir_operand 	dest,
	_in ir_operand 	src1
){
	return I64_IR_FORM_MAP[dest.id][src1.id];
}

/*
	Resolve I64 opcode based on IR 
*/
_inline_force enum i64_opcode _i64_ir_opcode_trans(
	_in u8 			ir_op_group,
	_in u8			ir_op_width,
	_in ir_operand 		dest,
	_in ir_operand 		src1
){
	// Resolve I64 generic opcode for the IR opcode
	enum i64_opcode i64_op_gen = 
		I64_IR_GROUP_TO_GENERIC[ir_op_group][ir_op_width];
	/*
		Read 32-bit unique value of the generic.

		Is needed later for identifying 'pool' of the opcodes
		that the opcode form resolves.
	*/
	u32 i64_op_gen_group = I64_OPCODE_VALUE(i64_op_gen);

	// Resolve I64 form for the IR operands
	enum i64_opcode_form i64_op_form = 
		_i64_ir_opcode_form_res(dest, src1);

	// Resolve specific I64 opcode for the I64 generic opcode and form
	return I64_IR_GENERIC_TO_SPECIFIC[i64_op_gen_group][i64_op_form];
}

/*	
 	Create I64 operand value and id from an IR operand
*/
_inline_force bool _i64_vid_from_ir(
	_in ir_operand		ir_op,
	_in_out i64_operand 	*i64_op 
){
	if (__builtin_expect(i64_op == nullptr, false)){
		return false;
	}

	switch(ir_op.id){
	case IR_OP_REG: // i64_op->desc.type & I64_REG
		i64_op->value = 0;
		// [value] field [ir_op] contains the unique register identifier
		i64_op->id = I64_REG_TO_OPERAND[ir_op.value];
		return true;
	case IR_OP_MEM:
		return true;
	case IR_OP_IMM:
		return true;
	default:
		return false;
	}
}

/*
 	Convert [ir_opcode] to IA-32e opcode.
	May generate multiple opcodes when needed.

	TODO:
	Make [opcodes] buffer bigger (or replace it)
	to allow custom routines for IR instructions
*/
bool i64_ir_opcode_conv(
	_in ir_raw_instr		instr,
	_in_out enum i64_opcode 	opcodes[2],
	_in bool			dest_mov
);

/*
 	Convert [ir_operand_set] to IA-32e operand array
*/
bool i64_ir_operand_conv(
	_in ir_raw_instr	instr,
	_in enum i64_opcode	opcode,
	_in_out i64_operand 	ops[I64_MAX_OP_COUNT],
	_in bool		dest_mov
);

