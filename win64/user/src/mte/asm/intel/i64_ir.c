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
	if (__builtin_expect(instr.set.ops_count > I64_MAX_OP_COUNT, false)){
		return (mte_raw_instr){0};
	}

	/*
		mov instruction required

		Destination register has to be the same as first source
		for instruction to be compliant with IR rules
		IR:
			dest = add i32 (src1), (src2)
		Intel64:
			dest = add (dest as src2), (src1)
	*/
	bool dest_mov =
		(instr.set.ops[0].id != instr.set.ops[1].id);
	// Unsupported
	if (dest_mov){
		io_i64(instr.set.ops[0].id);
		io_str(u"");
		io_i64(instr.set.ops[1].id);
		io_str(u"");
		ax_log_msg(AX_NOT_IMP, u"Assertion failed!");
		asrt(false);
	}

	mte_raw_instr buf = {0};

	enum i64_opcode opcode_buf[2] = {0};
	i64_operand operand_buf[2][I64_MAX_OP_COUNT] = {0};

	/*
		Lift from IR to I64 byte representation
	*/
	if (__builtin_expect(
		!i64_ir_opcode_conv(ir,
			instr,
			opcode_buf,
			operand_buf,
			dest_mov),
		false)
	){
		return (mte_raw_instr){0};
	}

	/*
	 	IMPORTANT!!!
		
	 	RIGHT NOW, payload cannot exceed 60 bytes.
		Single Intel64 instruction can take up to 16 bytes.
	*/
	u8 *payload_ptr = (u8*)buf.payload;

	u8 l0 = 0;
	u8 l1 = 0;

	const u8 buf_i = !!dest_mov;

	// Will never be the case for now
	/*if (dest_mov){
		axcheck_r(i64_emit_64(
				opcode_buf[0],
				operand_buf[0],
				payload_ptr,
				&l0),
			(mte_raw_instr){0}
		);
		// Offset the pointer by first length
		payload_ptr = offp(payload_ptr, l0);
	}*/

	axcheck_r(i64_emit_64(
			opcode_buf[buf_i],
			operand_buf[buf_i],
			payload_ptr,
			&l1),
		(mte_raw_instr){0}
	);

	// Count bytes in payload
	*len = l0 + l1;

	return buf;
}
#include "mte/perf.h"

bool i64_ir_opcode_conv(
	_in ir_context			*ir,
	_in ir_raw_instr		ir_instr,
	_in_out enum i64_opcode 	opcode_buf[2],
	_in_out i64_operand 		op_buf[2][I64_MAX_OP_COUNT],
	_in bool			dest_mov
){
	if (__builtin_expect(ir == nullptr, false)){
		return false;
	}
	if (__builtin_expect(ir_instr.opcode == IR_INVALID_OPCODE, false)){
		return false;
	}
	if (__builtin_expect(opcode_buf == nullptr, false)){
		return false;
	}

	// Decode IR opcode metadata
	u8 ir_op_width = IR_OPCODE_WIDTH(ir_instr.opcode);
	u8 ir_op_group = IR_OPCODE_GROUP(ir_instr.opcode);

	// Operand and opcode buffer index
	const u8 buf_i = !!dest_mov;

	/*
	 	Example of case with operand movement:

		IR:
		r0 = add i32 r1, r2

		Intel64:
		mov dword r0, r1
		add r0, r2
	*/
	if (dest_mov){
 		// Create mov from r1 to r0
		enum i64_opcode opcode = _i64_ir_opcode_trans(
			IR_GROUP_MOV,
			ir_op_width,
			ir_instr.set.ops[0],
			ir_instr.set.ops[1]);
		i64_opcode_desc opcode_desc = _lookup_opcode_meta(opcode);

		/*
			Convert the destination operand (r0)
		*/
		if (__builtin_expect(
			!_i64_op_from_ir(ir,
				ir_instr.set.ops[0],
				opcode_desc.ops[0],
				&op_buf[0][0]),
			false)
		){
			return false;
		}

		/*
			Convert the source operand (r1)
		*/
		if (__builtin_expect(
			!_i64_op_from_ir(ir,
				ir_instr.set.ops[1],
				opcode_desc.ops[1],
				&op_buf[0][1]),
			false)
		){
			return false;
		}
		opcode_buf[buf_i] = opcode;
	}

	// Operand index of the IR
	u8 ir_op_i = 2;
	// Operand sub-buffer index
	u8 op_i = 1;

	// Translate the actual opcode
	enum i64_opcode opcode = _i64_ir_opcode_trans(
		ir_op_group,
		ir_op_width,
		ir_instr.set.ops[0],
		ir_instr.set.ops[ir_op_i]);
	i64_opcode_desc opcode_desc =
		_lookup_opcode_meta(opcode);

	/*
	 	Translate destination (r0) operand
	*/
	if (__builtin_expect(
		!_i64_op_from_ir(ir,
			ir_instr.set.ops[0],
			opcode_desc.ops[0],
			&op_buf[buf_i][0]),
		false)
	){
		return false;
	}

	/*
		Iterate and convert the rest of the operands in the [instr.set]
	*/
	for (;op_i < opcode_desc.ops_count; op_i++, ir_op_i++){
		if (__builtin_expect(
			!_i64_op_from_ir(ir,
				ir_instr.set.ops[ir_op_i],
				opcode_desc.ops[op_i],
				&op_buf[buf_i][op_i]),
			false)
		){
			return false;
		}
	}
	opcode_buf[buf_i] = opcode;

	return true;
}

