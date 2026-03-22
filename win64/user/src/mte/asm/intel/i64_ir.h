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

/*
   	============ IMPORTANT ============
 	Expects IA-32e compliant IR instruction

	Resolve instruction form based on first 2 operands
*/
_inline_force enum i64_opcode_form _i64_ir_opcode_form_res(
	_in ir_operand_set 	set
){
	ir_operand op0 = set.ops[0];
	ir_operand op1 = set.ops[1];

	return I64_IR_FORM_MAP[op0.value][op1.value];
}

/*
   	============ IMPORTANT ============
 	Expects IA-32e compliant IR instruction

 	Convert [ir_opcode] to IA-32e opcode
*/
enum i64_opcode i64_ir_opcode_conv(
	_in ir_raw_instr	instr
);

/*
 	Convert [ir_operand_set] to IA-32e operand array
*/
bool i64_ir_set_conv(
	_in ir_operand_set	ir_ops,
	_in_out i64_operand 	i64_ops[4]
);

