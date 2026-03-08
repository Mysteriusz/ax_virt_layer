#include "mte/ir/ir.h"

#include "intel/emitter/i64_emit.h"

#include "i64.h"

axres i64_raw_to_ir(
	_in i64_mte_raw_instr	instr,
	_in_out ir_raw_instr		*buf // Initialize before use by init_ir_raw_instr
);
axres i64_ir_to_raw(
	_in ir_raw_instr		instr,
	_in_out i64_mte_raw_instr	*buf // Initialize before use by init_ir_raw_instr
);

u8 i64_role_to_reg(
	_in enum cpu_reg_role 	role
);

