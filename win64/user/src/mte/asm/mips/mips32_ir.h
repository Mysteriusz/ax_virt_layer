#include "mte/asm/mips/mips32.h"

axres mips32_raw_to_ir(
	_in mips32_mte_raw_instr	instr,
	_in const ir_rule		*rule,
	_in_out ir_raw_instr		*buf // Initialize before use by init_ir_raw_instr
);

