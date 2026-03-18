#include "i64_ir.h"

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

	const struct ir_context_desc *desc = &ir->desc;

	return (mte_raw_instr){0};
}

