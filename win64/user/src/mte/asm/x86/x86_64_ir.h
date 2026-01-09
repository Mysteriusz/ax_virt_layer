#include "mte/asm/x86/x86_64.h"

axres x86_64_raw_to_ir(
	_in x86_64_mte_raw_instr	instr,
	_in const ir_rule		*rule,
	_in_out ir_raw_instr		*buf // Initialize before use by init_ir_raw_instr
);

