#include <ax_type.h>

#include "intel64_operand.h"

#define INTEL64_MAX_OPCODE_LEN 	4

// Opcode from value initializer (shouldn`t be used externaly)
#define intel64_oi(l, v) ((l##ULL << 56) | (v))

/*
   	Reference used:
		- http://ref.x86asm.net/coder64.html
 	
 	How are opcode enums structured?
	As expected first word is the opcode mnemonic itself.
	Then each word describes one operator.

	Structure follows: (ignore [ and ])
		[dest/source prefix][max bit width]

	The only exception for the structure is a register.
	For example if word is AL or AX

	Table of dest/source:
		R -> register
		M -> memory
		IMM -> immidiate

	Table of max bit width:
		Without explicit prefix it`s automatically RM
		8 -> register/memory 8 bits
		16 -> register/memory 16 bits
		32 -> register/memory 8/16/32 bits
		64 -> register/memory 8/16/32/64 bits

	Each opcode`s MSB is it`s length in bytes
*/
enum intel64_opcode : u64{
	ADD_8_R8 	= intel64_oi(1, 0x00), 	// ADD	r/m8		r8
	ADD_64_R64 	= intel64_oi(1, 0x01), 	// ADD	r/m16/32/64	r16/32/64
	ADD_R8_8 	= intel64_oi(1, 0x02), 	// ADD	r8		r/m8
	ADD_R64_64 	= intel64_oi(1, 0x03),	// ADD	r16/32/64	r/m16/32/64
	ADD_AL_IMM8 	= intel64_oi(1, 0x04), 	// ADD	AL		imm8
	ADD_AX_IMM32 	= intel64_oi(1, 0x05), 	// ADD	rAX		imm16/32
};

/*
	FOR SIMD THERE SHOULD BE A SEPARATE META
*/
struct intel64_opcode_meta{
	struct intel64_operand_desc 	desc[INTEL64_RED_OP_COUNT];
	u8 				op_count;
};

/*
 	Table of metadata structs of only 1 byte length opcode
*/
static struct intel64_opcode_meta L1_OPCODE_META_TABLE[0xff] = {
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

/*
	Opcode length to opcode metadata array table
*/
static struct intel64_opcode_meta *LN_OPCODE_META_TABLE[INTEL64_MAX_OPCODE_LEN] = {
	[0] = L1_OPCODE_META_TABLE,
};

