#include "mips/mips32_opcode.h"

ir_opcode MIPS32_TO_IR_TABLE[0xff] = {
	[0 ... 0xfe] = IR_INVALID_OPCODE,
	[0b100000] = IR_ADD_I32,
	[0b100101] = IR_OR_I32,
	[0b100100] = IR_AND_I32,
	[0b100010] = IR_SUB_I32,
};

