#include "intel/i64_cpu.h"

const u64 _I64_CPU_REG_ROLE_MAP[REG_ROLE_MAX][4] = {
	[REG_RETURN]    = { 0x0001ULL, 0ULL, 0ULL, 0ULL }, // root[0]
	[REG_STACK]     = { 0x03C6ULL, 0ULL, 0ULL, 0ULL }, // root[1, 2, 6, 7, 8, 9]
	[REG_PRESERVE]  = { 0xF008ULL, 0ULL, 0ULL, 0ULL }, // root[3, 12, 13, 14, 15]
	[REG_STACK_PTR] = { 0x0010ULL, 0ULL, 0ULL, 0ULL }, // root[4]
	[REG_FRAME_PTR] = { 0x0020ULL, 0ULL, 0ULL, 0ULL }, // root[5]
	[REG_TEMP]      = { 0x0C00ULL, 0ULL, 0ULL, 0ULL }, // root[10, 11]
};

const struct cpu_reg_map _I64_CPU_REG_MAP = {
	.reg_count = I64_REG_COUNT,
	.reg_width = I64_REG_WIDTH,
	.root = (struct cpu_reg_desc[I64_REG_COUNT]){
		[0]  = {.id = I64_rAX & 0x1f, .role = REG_RETURN,  	.value = 0},
		[1]  = {.id = I64_rCX & 0x1f, .role = REG_STACK,  	.value = 0},
		[2]  = {.id = I64_rDX & 0x1f, .role = REG_STACK,  	.value = 0},
		[3]  = {.id = I64_rBX & 0x1f, .role = REG_PRESERVE,  	.value = 0},
		[4]  = {.id = I64_rSP & 0x1f, .role = REG_STACK_PTR,  	.value = 0},
		[5]  = {.id = I64_rBP & 0x1f, .role = REG_FRAME_PTR,  	.value = 0},
		[6]  = {.id = I64_rSI & 0x1f, .role = REG_STACK,  	.value = 0},
		[7]  = {.id = I64_rDI & 0x1f, .role = REG_STACK,  	.value = 0},
		[8]  = {.id = I64_r8  & 0x1f, .role = REG_STACK,  	.value = 0},
		[9]  = {.id = I64_r9  & 0x1f, .role = REG_STACK,  	.value = 0},
		[10] = {.id = I64_r10 & 0x1f, .role = REG_TEMP,  	.value = 0},
		[11] = {.id = I64_r11 & 0x1f, .role = REG_TEMP,		.value = 0},
		[12] = {.id = I64_r12 & 0x1f, .role = REG_PRESERVE,  	.value = 0},
		[13] = {.id = I64_r13 & 0x1f, .role = REG_PRESERVE,  	.value = 0},
		[14] = {.id = I64_r14 & 0x1f, .role = REG_PRESERVE,  	.value = 0},
		[15] = {.id = I64_r15 & 0x1f, .role = REG_PRESERVE,	.value = 0},
	},
	.role_map = &_I64_CPU_REG_ROLE_MAP,
};

