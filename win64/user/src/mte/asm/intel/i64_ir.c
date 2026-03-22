#include "i64_ir.h"

#include "intel/emitter/i64_emit.h"

ir_raw_instr i64_raw_to_ir(
	_in mte_raw_instr 	instr,
	_in ir_context 		*ir,
	_out u8			*len // Original instruction length (in bytes)
){
	unref(instr);
	unref(ir);
	unref(len);
	return (ir_raw_instr){0};
}

mte_raw_instr i64_ir_to_raw(
	_in ir_raw_instr 	instr,
	_in ir_context 		*ir,
	_out u8			*len // Target instruction length (in bytes)
){
	if (__builtin_expect(ir == nullptr, false)){
		return (mte_raw_instr){0};
	}
	if (__builtin_expect(len == nullptr, false)){
		return (mte_raw_instr){0};
	}
	// Check operand count
	if (__builtin_expect(instr.set.op_count > I64_MAX_OP_COUNT, false)){
		return (mte_raw_instr){0};
	}

	bool has_ret =
		IR_OPCODE_HAS_RETURN(instr.opcode);
	unref(has_ret);

	// Requires mov instruction before
	bool coercing = (instr.set.ops[0].value != instr.set.ops[1].value);

	struct ir_context_desc *desc = &ir->desc;
	enum i64_opcode opcode =
		i64_ir_opcode_conv(instr);

	/*i64_ir_set_conv(
		instr.set,
		ops
	);

	i64_emit_64(
		ADD_64_R64,
		ops,
		(u8*)desc->gen_ptr
	);
	desc->gen_ptr = offp(desc->gen_ptr, 3);*/

	return (mte_raw_instr){0};
}
#include "mte/perf.h"

// Expects I64 compliant register ordering
enum i64_opcode i64_ir_opcode_conv(
	_in ir_raw_instr	instr
){
	// Decode IR opcode metadata
	u8 ir_op_width = IR_OPCODE_WIDTH(instr.opcode);
	u8 ir_op_group = IR_OPCODE_GROUP(instr.opcode);

	if (__builtin_expect(instr.opcode == IR_INVALID_OPCODE, false)){
		return I64_INVALID_OPCODE;
	}

	// Resolve generic opcode for the IR opcode
	enum i64_opcode i64_op_gen = 
		I64_IR_GROUP_TO_GENERIC[ir_op_group][ir_op_width];
	u64 i64_op_gen_value = I64_OPCODE_VALUE(i64_op_gen);

	// Resolve form required by the IR operands
	enum i64_opcode_form i64_op_form = 
		_i64_ir_opcode_form_res(instr.set);

	// Resolve specific opcode for the generic opcode
	enum i64_opcode i64_op_spec = 
		I64_IR_GENERIC_TO_SPECIFIC[i64_op_gen_value][i64_op_form];

	return i64_op_spec;
}
bool i64_ir_set_conv(
	_in ir_operand_set	set,
	_in_out i64_operand 	ops[4]
){
	if (__builtin_expect(ops == nullptr, false)){
		return false;
	}

	return true;
}

