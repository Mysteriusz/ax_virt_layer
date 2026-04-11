#include "i64_opcode.h"

/*
 	Each entry index is it`s leading byte 
	(The one that defines the opcode)
*/
i64_opcode_desc L1_OPCODE_META_TABLE[0xff] = {
	[ADD_8_R8 & 0xff] = {
		.ops_count = 2,
		.ops[0].type = I64_REG | I64_MEM,
		.ops[0].width = W8,
		.ops[1].type = I64_REG,
		.ops[1].width = W8,
		.flags = MODRM | LEGACY,
		.prefix = I64_LOCK_PREF,
		.form = FORM_RM_R,
	},
	[ADD_64_R64 & 0xff] = {
		.ops_count = 2,
		.ops[0].type = I64_REG | I64_MEM,
		.ops[0].width = W64,
		.ops[1].type = I64_REG,
		.ops[1].width = W64,
		.flags = REX | MODRM | LEGACY,
		.prefix = I64_LOCK_PREF,
		.form = FORM_RM_R,
	},
	[ADD_R8_8 & 0xff] = {
		.ops_count = 2,
		.ops[0].type = I64_REG,
		.ops[0].width = W8,
		.ops[1].type = I64_REG | I64_MEM,
		.ops[1].width = W8,
		.flags = MODRM,
		.prefix = I64_NO_PREF,
		.form = FORM_R_RM,
	},
	[ADD_R64_64 & 0xff] = {
		.ops_count = 2,
		.ops[0].type = I64_REG,
		.ops[0].width = W64,
		.ops[1].type = I64_REG | I64_MEM,
		.ops[1].width = W64,
		.flags = REX | MODRM,
		.prefix = I64_NO_PREF,
		.form = FORM_R_RM,
	},
	[ADD_AL_IMM8 & 0xff] = {
		.ops_count = 2,
		.ops[0].type = I64_REG,
		.ops[0].width = W8,
		.ops[1].type = I64_IMM,
		.ops[1].width = W8,
		.flags = 0,
		.prefix = I64_NO_PREF,
		.form = FORM_rAX_IMM,
	},
	[ADD_rAX_IMM32 & 0xff] = {
		.ops_count = 2,
		.ops[0].type = I64_REG,
		.ops[0].width = W32,
		.ops[1].type = I64_IMM,
		.ops[1].width = W32,
		.flags = 0,
		.prefix = I64_NO_PREF,
		.form = FORM_rAX_IMM,
	},
};
