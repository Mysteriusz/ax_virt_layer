#include "mte/perf.h"

#include "intel/i64_rex.h"

#include "i64_emit.h"

axres i64_emit_64(
	_in enum i64_opcode 	opcode,
	_in i64_operand 		ops[I64_RED_OP_COUNT],
	_out i64_mte_raw_instr	*buf
){
	__INL_PERF_INIT
	__INL_PERF_START
	if (ops == nullptr){
		return AX_INV_ARG;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	// Read most significant byte of the opcode (length) based on [MTE_I64_OPCODE_INT.I64_OI]
	u8 opcode_len = ((opcode >> 56) & 0xff);
	if (opcode_len > I64_MAX_OPCODE_LEN){
		return AX_INV_CODE;
	}

	struct i64_opcode_meta opcode_meta = {0};
	switch(opcode_len){
	case 1:
		// Map [opcode] to opcode meta table for 1 byte opcodes
		opcode_meta = L1_OPCODE_META_TABLE[opcode & 0xff];
		break;
	default:
		return AX_INV_DATA;
	}

	/*
	 	Intialize dummy opcode with provided [opcode_meta]
	*/
	struct i64_opcode_meta opcode_match = (struct i64_opcode_meta){
		.op_count = opcode_meta.op_count,
		.flags = opcode_meta.flags
	};

	/*
	 	Check opcode operands and fill [opcode_match] descriptors
	*/
	for (u32 i = 0; i < opcode_meta.op_count; i++){
		// Check operand compatiblity with meta
		if (!_i64_operand_cmp(opcode_meta.desc[i], ops[i].desc)){
			return AX_UNK_ERR;
		}
		// Copy descriptor to the dummy
		opcode_match.desc[i] = ops[i].desc;
	}

	i64_mte_raw_instr instr = {0};

	/*
	 	Construct the dummy opcode
	*/

	u8 rex = _i64_rex_resolve(opcode_match);

	*buf = instr;

	__INL_PERF_END
	__INL_PERF_LOG

	ax_log(rex);

	return AX_SUCC;
}

