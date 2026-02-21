#include "intel64_emit.h"

axres intel64_emit_64(
	_in enum intel64_opcode 	opcode,
	_in intel64_operand 		ops[INTEL64_MAX_OP_COUNT],
	_in u8				op_count,
	_out intel64_mte_raw_instr	*instr
){
	if (ops == nullptr){
		return AX_INV_ARG;
	}
	if (instr == nullptr){
		return AX_INV_BUF;
	}

	u8 opcode_len = (opcode & (0xffULL << 56)) >> 56;
	if (opcode_len > INTEL64_MAX_OPCODE_LEN){
		return 0;
	}

	// Map opcode length - 1 to index of length tables
	struct intel64_opcode_meta *opcode_meta =
		LN_OPCODE_META_TABLE[opcode_len - 1];

	for (u8 i = 0; i < op_count; i++){
		// Check operands compatibility
		if (!_intel64_operand_cmp(
			opcode_meta->desc[i],
			ops[i].desc)
		){
			return AX_INV_DATA;
		}
	}

	return AX_SUCC;
}

