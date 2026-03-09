#include "mips/mips32_instr.h"

#include "mips32_qtables.h"

void mips32_load_qtables(
	void
){
	MIPS32_TO_IR_TABLE[0b100000] = IR_ADD_I32; // add
}

