#include "mte/ir/ir.h"

#include "intel/emitter/intel64_emit.h"

#include "intel64.h"

axres intel64_raw_to_ir(
	_in intel64_mte_raw_instr	instr,
	_in_out ir_raw_instr		*buf // Initialize before use by init_ir_raw_instr
);
axres intel64_ir_to_raw(
	_in ir_raw_instr		instr,
	_in_out intel64_mte_raw_instr	*buf // Initialize before use by init_ir_raw_instr
);

