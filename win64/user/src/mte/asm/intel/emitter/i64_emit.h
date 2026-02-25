#if !defined(MTE_I64_EMIT_INT)
#define MTE_I64_EMIT_INT

#include "intel/instr/i64_opcode.h"
#include "intel/instr/i64_instr.h"

#include "i64_leg.h"
#include "i64_rex.h"
#include "i64_immd.h"
#include "i64_modrm.h"

/*
 	Emit IA-32e 64-bit instruction
*/
axres i64_emit_64(
	_in enum i64_opcode 		opcode, // Opcode to emit
	_in i64_operand 		ops[I64_RED_OP_COUNT], // Operands to use
	_out i64_mte_raw_instr		*buf
);

#endif // !defined(MTE_I64_EMIT_INT)

