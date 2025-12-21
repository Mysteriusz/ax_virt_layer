#include "mte/core.h"
#include "mte/ir.h"

axres mips32_raw_to_ir(
	_in mips32_mte_raw_instr	instr,
	_in_out ir_raw_instr		*buf // Initialize before use by init_ir_raw_instr
);

