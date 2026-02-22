#include "intel64_opcode.h"

struct intel64_opcode_meta L1_OPCODE_META_TABLE[0xff] = {
	[ADD_8_R8 & 0xff] = {
		.op_count = 2,
		.desc[0].type = INTEL64_REG_MEM,
		.desc[0].width = W8,
		.desc[1].type = INTEL64_REG,
		.desc[1].width = W8
	},
	[ADD_64_R64 & 0xff] = {
		.op_count = 2,
		.desc[0].type = INTEL64_REG_MEM,
		.desc[0].width = W64,
		.desc[1].type = INTEL64_REG,
		.desc[1].width = W64
	},
	[ADD_R8_8 & 0xff] = {
		.op_count = 2,
		.desc[0].type = INTEL64_REG,
		.desc[0].width = W8,
		.desc[1].type = INTEL64_REG_MEM,
		.desc[1].width = W8
	},
	[ADD_R64_64 & 0xff] = {
		.op_count = 2,
		.desc[0].type = INTEL64_REG,
		.desc[0].width = W64,
		.desc[1].type = INTEL64_REG_MEM,
		.desc[1].width = W64
	},
	[ADD_AL_IMM8 & 0xff] = {
		.op_count = 2,
		.desc[0].type = INTEL64_REG,
		.desc[0].width = W8,
		.desc[1].type = INTEL64_IMM,
		.desc[1].width = W8
	},
	[ADD_AX_IMM32 & 0xff] = {
		.op_count = 2,
		.desc[0].type = INTEL64_REG,
		.desc[0].width = W16,
		.desc[1].type = INTEL64_IMM,
		.desc[1].width = W32
	},
};
