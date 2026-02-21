#include "intel/instr/intel64_opcode.h"
#include "intel/instr/intel64_instr.h"
#include "intel/intel64.h"

/*
 	Emit IA-32e 64-bit instruction
*/
axres intel64_emit_64(
	_in enum intel64_opcode 	opcode,
	_in intel64_operand 		ops[INTEL64_MAX_OP_COUNT],
	_in u8				op_count,
	_out intel64_mte_raw_instr	*instr
);

