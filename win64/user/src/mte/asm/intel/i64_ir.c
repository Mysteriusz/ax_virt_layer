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
		(instr.set.ops[0].value != instr.set.ops[1].value);

	struct ir_context_desc *desc = &ir->desc;

	enum i64_opcode opcode_buf[2] = {0};
	i64_operand operand_buf[2][I64_MAX_OP_COUNT] = {0};

	i64_ir_opcode_conv(
		instr,
		opcode_buf,
		dest_mov);
	i64_ir_operand_conv(
		instr,
		opcode_buf[0],
		operand_buf[0],
		dest_mov);

	i64_emit_64(
		opcode_buf[0],
		operand_buf[0],
		(u8*)desc->gen_ptr
	);

	return (mte_raw_instr){0};
}
#include "mte/perf.h"

bool i64_ir_opcode_conv(
	_in ir_raw_instr		instr,
	_in_out enum i64_opcode 	opcodes[2],
	_in bool			dest_mov
){
	// Decode IR opcode metadata
	u8 ir_op_width = IR_OPCODE_WIDTH(instr.opcode);
	u8 ir_op_group = IR_OPCODE_GROUP(instr.opcode);

	if (__builtin_expect(instr.opcode == IR_INVALID_OPCODE, false)){
		return false;
	}
	if (__builtin_expect(opcodes == nullptr, false)){
		return false;
	}

	// Generated opcode buffer index
	u8 gen_i = 0;

	// Operand 0/1 index
	u8 op0_i = 0;
	u8 op1_i = 1;

	/*
	 	Example of case with operand movement:

		IR:
		r0 = add i32 r1, r2

		Intel64:
		mov dword r0, r1
		add r0, r2
	*/
	if (dest_mov){
 		// Translate mov from r1 to r0
		opcodes[gen_i] = _i64_ir_opcode_trans(
			IR_GROUP_MOV,
			ir_op_width,
			instr.set.ops[0],
			instr.set.ops[1]);
		op1_i = 2;
		gen_i++;
	}

	// Translate the actual opcode
	opcodes[gen_i] = _i64_ir_opcode_trans(
		ir_op_group,
		ir_op_width,
		instr.set.ops[op0_i],
		instr.set.ops[op1_i]);

	return true;
}

bool i64_ir_operand_conv(
	_in ir_raw_instr	ir_instr,
	_in enum i64_opcode	opcode,
	_in_out i64_operand 	ops[I64_MAX_OP_COUNT],
	_in bool		dest_mov
){
	i64_opcode_desc opcode_desc =
		_lookup_opcode_meta(opcode); 

	if (__builtin_expect(ops == nullptr, false)){
		return false;
	}
	if (__builtin_expect(opcode == I64_INVALID_OPCODE, false)){
		return false;
	}
	if (__builtin_expect(ir_instr.opcode == IR_INVALID_OPCODE, false)){
		return false;
	}

	for (u8 i = 0; i < opcode_desc.ops_count; i++){
		ops[i].desc = opcode_desc.ops[i];

		bool vid = _i64_vid_from_ir(ir_instr.set.ops[i + !dest_mov], &ops[i]);
		if (__builtin_expect(!vid, false)){
			return false;
		}
	}

	return true;
}

