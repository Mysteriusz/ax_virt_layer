#include "mips32_qtables.h"

/*
 	For MTE_MIPS32_IR_INT interface.
*/
ir_opcode MIPS32_IR_TABLE[0xff] = {IR_INVALID_OPCODE};

void mips32_load_qtables(
	void
){
	MIPS32_IR_TABLE[0b100000] = IR_ADD_I32; // add
}

