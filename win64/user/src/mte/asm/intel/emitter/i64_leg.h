#include <ax_type.h>

#include <ax_type.h>
#include <ax_io.h>

#include "intel/instr/i64_opcode.h"
#include "intel/instr/i64_operand.h"
#include "intel/i64_reg.h"

u8 i64_leg_resolve(
	_in enum i64_opcode_flags	flags,
	_in u8				op_count,
	_in i64_operand			ops[I64_RED_OP_COUNT]
);

