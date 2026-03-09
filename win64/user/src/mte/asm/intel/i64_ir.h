#include "mte/ir/ir.h"
#include "mte/cpu.h"

#include "intel/instr/i64_instr.h"
#include "intel/instr/i64_operand.h"

axres i64_raw_to_ir(
	_in i64_mte_raw_instr	instr,
	_in_out ir_raw_instr		*buf // Initialize before use by init_ir_raw_instr
);
axres i64_ir_to_raw(
	_in ir_raw_instr		instr,
	_in_out i64_mte_raw_instr	*buf // Initialize before use by init_ir_raw_instr
);

_inline_avert void i64_load_cpu_reg_map(
	void
);

