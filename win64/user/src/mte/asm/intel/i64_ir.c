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
	_in ir_raw_instr 	ir_instr,
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
	if (__builtin_expect(ir_instr.set.ops_count > I64_MAX_OP_COUNT, false)){
		return (mte_raw_instr){0};
	}

	u8 ir_flags = IR_OPCODE_FLAGS(ir_instr.opcode);

	/*
		mov instruction required

		Destination register has to be the same as first source
		for instruction to be compliant with IR rules
		IR:
			dest = add i32 (src1), (src2)
		Intel64:
			dest = add (dest as src2), (src1)

	*/
	bool non_dest_mov = (ir_instr.set.ops[0].id == ir_instr.set.ops[1].id) &&
		(ir_flags & IR_DEST_NEQ_SRC);

	asrt(!non_dest_mov, ax_log_msg(AX_NOT_IMP,
		u"I64 unsupported ISA operand order"));

	mte_raw_instr buf = {0};

	enum i64_opcode i64_opcode = {0};
	i64_operand i64_operands[I64_MAX_OP_COUNT] = {0};

	/*
		Lift from IR to I64 byte representation
	*/
	if (__builtin_expect(
		!i64_ir_opcode_conv(ir_instr,
			&i64_opcode,
			&i64_operands),
		false)
	){
		return (mte_raw_instr){0};
	}

	/*
	 	Emit the instruciton into the payload
	*/
	axcheck_r(i64_emit_64(
			i64_opcode,
			i64_operands,
			buf.payload,
			len),
		(mte_raw_instr){0}
	);

	return buf;
}
#include "mte/perf.h"

bool i64_ir_opcode_conv(
	_in ir_raw_instr		ir_instr,
	_out enum i64_opcode 		*opcode,
	_out i64_operand 		(*operand_buf)[I64_MAX_OP_COUNT]
){
	if (__builtin_expect(opcode == nullptr, false)){
		return false;
	}
	if (__builtin_expect(operand_buf == nullptr, false)){
		return false;
	}

	// Decode IR opcode metadata
	u8 ir_op_width = IR_OPCODE_WIDTH(ir_instr.opcode);
	u8 ir_op_group = IR_OPCODE_GROUP(ir_instr.opcode);
	u8 ir_op_dest_src = !!(ir_instr.set.ops[0].id == ir_instr.set.ops[1].id);

	// Translate the opcode
	*opcode = _i64_ir_opcode_trans(
		ir_op_group,
		ir_op_width,
		ir_instr.set.ops[ir_op_dest_src],
		ir_instr.set.ops[ir_op_dest_src + 1]);

	// Lookup metadata for the opcode
	i64_opcode_desc opcode_desc =
		_i64_lookup_opcode_meta(*opcode);

	/*
	 	TODO: DO AN OPS COUNT VALIDATION
	*/

	/*
		Iterate and convert all of the operands in the [ir_instr.set]
	*/
	for (u8 op_i = ir_op_dest_src; op_i < ir_instr.set.ops_count; op_i++){
		if (__builtin_expect(
			!_i64_ir_operand_trans(
				ir_instr.set.ops[op_i],
				opcode_desc.ops[op_i - ir_op_dest_src],
				&(*operand_buf)[op_i - ir_op_dest_src]),
			false)
		){
			return false;
		}
	}

	return true;
}

