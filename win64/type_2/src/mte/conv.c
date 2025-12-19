#include "mte/conv.h"
#include "mte/mips32.h"

axres mte_byte_to_raw(
	_in mte_byte_instr	*instr,
	_in mte_raw_instr	*buf
){
	if (instr == nullptr
	|| buf == nullptr){
		return AX_INV_ARG;
	}

	switch(instr->arch){
	case mips32:
		mips32_instr_type(
			mips32_opcode(instr))

		struct mte_raw_instr_mips32_r;
		break;
	default:
		return AX_MTE_INV_ARCH;
	}

	return AX_SUCC;
}

