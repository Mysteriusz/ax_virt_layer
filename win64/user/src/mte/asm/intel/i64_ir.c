#include "i64_ir.h"

axres i64_raw_to_ir(
	_in i64_mte_raw_instr	instr,
	_in_out ir_raw_instr		*buf // Initialize before use by init_ir_raw_instr
){
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	/*switch(_i64_check_type(instr)){
	default:
		return 0;
	}*/
	return 0;
}

axres i64_ir_to_raw(
	_in ir_raw_instr		instr,
	_in_out i64_mte_raw_instr	*buf // Initialize before use by init_ir_raw_instr
){
	if (buf == nullptr){
		return AX_INV_BUF;
	}
	return AX_SUCC;
}

struct cpu_reg_map _I64_CPU_REG_MAP = {.root = (struct cpu_reg_desc[32]){0}};

_inline_avert void i64_load_cpu_reg_map(
	void
){
    _I64_CPU_REG_MAP.reg_count = 16;
    _I64_CPU_REG_MAP.reg_width = 64;
    _I64_CPU_REG_MAP.root[0]  = (struct cpu_reg_desc){.id = I64_rAX & 0x1f, .role = REG_RETURN};
    _I64_CPU_REG_MAP.root[1]  = (struct cpu_reg_desc){.id = I64_rCX & 0x1f, .role = REG_STACK};
    _I64_CPU_REG_MAP.root[2]  = (struct cpu_reg_desc){.id = I64_rDX & 0x1f, .role = REG_STACK};
    _I64_CPU_REG_MAP.root[3]  = (struct cpu_reg_desc){.id = I64_rBX & 0x1f, .role = REG_PRESERVE};
    _I64_CPU_REG_MAP.root[4]  = (struct cpu_reg_desc){.id = I64_rSP & 0x1f, .role = REG_STACK_PTR};
    _I64_CPU_REG_MAP.root[5]  = (struct cpu_reg_desc){.id = I64_rBP & 0x1f, .role = REG_FRAME_PTR};
    _I64_CPU_REG_MAP.root[6]  = (struct cpu_reg_desc){.id = I64_rSI & 0x1f, .role = REG_STACK};
    _I64_CPU_REG_MAP.root[7]  = (struct cpu_reg_desc){.id = I64_rDI & 0x1f, .role = REG_STACK};
    _I64_CPU_REG_MAP.root[8]  = (struct cpu_reg_desc){.id = I64_r8  & 0x1f, .role = REG_STACK};
    _I64_CPU_REG_MAP.root[9]  = (struct cpu_reg_desc){.id = I64_r9  & 0x1f, .role = REG_STACK};
    _I64_CPU_REG_MAP.root[10] = (struct cpu_reg_desc){.id = I64_r10 & 0x1f, .role = REG_TEMP};
    _I64_CPU_REG_MAP.root[11] = (struct cpu_reg_desc){.id = I64_r11 & 0x1f, .role = REG_TEMP};
    _I64_CPU_REG_MAP.root[12] = (struct cpu_reg_desc){.id = I64_r12 & 0x1f, .role = REG_PRESERVE};
    _I64_CPU_REG_MAP.root[13] = (struct cpu_reg_desc){.id = I64_r13 & 0x1f, .role = REG_PRESERVE};
    _I64_CPU_REG_MAP.root[14] = (struct cpu_reg_desc){.id = I64_r14 & 0x1f, .role = REG_PRESERVE};
    _I64_CPU_REG_MAP.root[15] = (struct cpu_reg_desc){.id = I64_r15 & 0x1f, .role = REG_PRESERVE};
}
