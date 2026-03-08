#if !defined(MTE_MIPS32_OPCODE_INT)
#define MTE_MIPS32_OPCODE_INT

#include "mte/ir/ir_instr.h"

/*
	Maps 8-bit mips32 opcode to IR opcode
*/

extern ir_opcode MIPS32_TO_IR_TABLE[0xff];

#endif // !defined(MTE_MIPS32_OPCODE_INT)

