#include "intel/instr/i64_opcode.h"
#include "intel/instr/i64_instr.h"
#include "intel/i64.h"

/*
 	Emit IA-32e 64-bit instruction
*/
axres i64_emit_64(
	_in enum i64_opcode 		opcode, // Opcode to emit
	_in enum i64_opcode_prefix	prefix, // Opcode flags (Have to be compatible with the opcode)
	_in i64_operand 		ops[I64_RED_OP_COUNT], // Operands to use
	_in_opt cpu_state 		*cpu,
	_out i64_mte_raw_instr		*buf
);

