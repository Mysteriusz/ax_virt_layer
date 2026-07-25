#include "i64_cpu.h"


struct cpu_reg_desc _I64_CPU_REG_ROOT[I64_REG_COUNT] = {0};
struct cpu_reg_map _I64_CPU_REG_MAP = {.root = _I64_CPU_REG_ROOT};

u16 _I64_CPU_REG_ROLE_MAP[0xff] = {0};

_inline_avert void i64_load_cpu_reg_map(
	void
){
	// Controlled UB
	*(u16*)&_I64_CPU_REG_MAP.reg_count = I64_REG_COUNT;
	*(u16*)&_I64_CPU_REG_MAP.reg_width = I64_REG_WIDTH;

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

	_I64_CPU_REG_MAP.role_map = (u16 (*)[])_I64_CPU_REG_ROLE_MAP;

	// Fill reg role mask
	_I64_CPU_REG_ROLE_MAP[REG_RETURN]    = 0x0001; // root[0]
	_I64_CPU_REG_ROLE_MAP[REG_STACK]     = 0x03C6; // root[1, 2, 6, 7, 8, 9]
	_I64_CPU_REG_ROLE_MAP[REG_PRESERVE]  = 0xF008; // root[3, 12, 13, 14, 15]
	_I64_CPU_REG_ROLE_MAP[REG_STACK_PTR] = 0x0010; // root[4]
	_I64_CPU_REG_ROLE_MAP[REG_FRAME_PTR] = 0x0020; // root[5]
	_I64_CPU_REG_ROLE_MAP[REG_TEMP]      = 0x0C00; // root[10, 11]
}

