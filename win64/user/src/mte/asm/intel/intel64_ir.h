#include "mte/ir/ir.h"

#include "intel64.h"

axres intel64_raw_to_ir(
	_in intel64_mte_raw_instr	instr,
	_in const ir_rule		*rule,
	_in_out ir_raw_instr		*buf // Initialize before use by init_ir_raw_instr
);

