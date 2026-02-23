#include "mte/perf.h"
#include "mte/cpu.h"

#include "intel/i64_rex.h"

#include "i64_emit.h"

axres i64_emit_64(
	_in enum i64_opcode 		opcode,
	_in enum i64_opcode_prefix	prefix,
	_in i64_operand 		ops[I64_RED_OP_COUNT],
	_in_opt cpu_state 		*cpu,
	_out i64_mte_raw_instr		*buf
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

	i64_opcode_desc opcode_desc = {0};
	switch(opcode_len){
	case 1:
		// Map [opcode] to opcode desc table for 1 byte opcodes
		opcode_desc = L1_OPCODE_META_TABLE[opcode & 0xff];
		break;
	default:
		return AX_INV_DATA;
	}

	/*
	 	Intialize dummy opcode with provided [opcode_desc]
	*/
	i64_opcode_desc opcode_match = (i64_opcode_desc){
		.op_count = opcode_desc.op_count,
		.flags = opcode_desc.flags
	};

	/*
	 	Check opcode operands and fill [opcode_match] descriptors
	*/
	for (u32 i = 0; i < opcode_desc.op_count; i++){
		// Check operand compatiblity with desc
		if (!_i64_operand_cmp(opcode_desc.desc[i], ops[i].desc)){
			return AX_UNK_ERR;
		}
		// Copy descriptor to the dummy
		opcode_match.desc[i] = ops[i].desc;
	}

	i64_mte_raw_instr instr = {0};

	/*
	 	Construct the dummy opcode
	*/

	u8 rex = _i64_rex_resolve(opcode_match, cpu);
	unref(prefix);

	*buf = instr;

	__INL_PERF_END
	__INL_PERF_LOG

	ax_log(rex);

	return AX_SUCC;
}

