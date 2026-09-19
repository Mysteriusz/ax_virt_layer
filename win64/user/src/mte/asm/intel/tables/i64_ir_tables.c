#include "intel/i64_ir.h"

/*
 	Each group requires generics for each bit-width in order:
	IR_8BIT
	IR_16BIT
	IR_32BIT
	IR_64BIT
*/
enum i64_opcode I64_IR_GROUP_TO_GENERIC[][8] = {
[IR_OPCODE_GROUP(IR_GROUP_ADD)] = {
		[IR_8BIT] = gADD_8_8,
		[IR_16BIT] = gADD_64_64,
		[IR_32BIT] = gADD_64_64, 
		[IR_64BIT] = gADD_64_64
	},
	[IR_OPCODE_GROUP(IR_GROUP_OR)] = {
		[IR_8BIT] = gOR_8_8,
		[IR_16BIT] = gOR_64_64,
		[IR_32BIT] = gOR_64_64,
		[IR_64BIT] = gOR_64_64,
	},
	[IR_OPCODE_GROUP(IR_GROUP_AND)] = {
		[IR_8BIT] = gAND_8_8,
		[IR_16BIT] = gAND_64_64,
		[IR_32BIT] = gAND_64_64,
		[IR_64BIT] = gAND_64_64,
	},
	[IR_OPCODE_GROUP(IR_GROUP_SUB)] = {
		[IR_8BIT] = gSUB_8_8,
		[IR_16BIT] = gSUB_64_64,
		[IR_32BIT] = gSUB_64_64,
		[IR_64BIT] = gSUB_64_64,
	},
	[IR_OPCODE_GROUP(IR_GROUP_MOV)] = {
		[IR_8BIT] = gMOV_8_8,
		[IR_16BIT] = gMOV_64_64,
		[IR_32BIT] = gMOV_64_64,
		[IR_64BIT] = gMOV_64_64,
	},
};

/*
 	Defines how generic opcode translates to an
	actual specific opcode given a form.
*/
enum i64_opcode I64_IR_GENERIC_TO_SPECIFIC[][16] = {
	[I64_OPCODE_VALUE(gADD_8_8)] = {
		[FORM_RM_R] = ADD_8_R8,
		[FORM_R_RM] = ADD_R8_8,
		[FORM_RM_IMM] = I64_INVALID_OPCODE, 
		[FORM_rAX_IMM] = I64_INVALID_OPCODE
	},
	[I64_OPCODE_VALUE(gADD_64_64)] = {
		[FORM_RM_R] = ADD_64_R64,
		[FORM_R_RM] = ADD_R64_64,
		[FORM_RM_IMM] = ADD_AL_IMM8, 
		[FORM_rAX_IMM] = ADD_rAX_IMM32
	},
	[I64_OPCODE_VALUE(gOR_8_8)] = {
		[FORM_RM_R] = OR_8_R8,
		[FORM_R_RM] = OR_R8_8,
		[FORM_RM_IMM] = I64_INVALID_OPCODE, 
		[FORM_rAX_IMM] = I64_INVALID_OPCODE
	},
	[I64_OPCODE_VALUE(gOR_64_64)] = {
		[FORM_RM_R] = OR_64_R64,
		[FORM_R_RM] = OR_R64_64,
		[FORM_RM_IMM] = OR_AL_IMM8, 
		[FORM_rAX_IMM] = OR_rAX_IMM32
	},
	[I64_OPCODE_VALUE(gAND_8_8)] = {
		[FORM_RM_R] = AND_8_R8,
		[FORM_R_RM] = AND_R8_8,
		[FORM_RM_IMM] = I64_INVALID_OPCODE, 
		[FORM_rAX_IMM] = I64_INVALID_OPCODE
	},
	[I64_OPCODE_VALUE(gAND_64_64)] = {
		[FORM_RM_R] = AND_64_R64,
		[FORM_R_RM] = AND_R64_64,
		[FORM_RM_IMM] = AND_AL_IMM8, 
		[FORM_rAX_IMM] = AND_rAX_IMM32
	},
	[I64_OPCODE_VALUE(gSUB_8_8)] = {
		[FORM_RM_R] = SUB_8_R8,
		[FORM_R_RM] = SUB_R8_8,
		[FORM_RM_IMM] = I64_INVALID_OPCODE, 
		[FORM_rAX_IMM] = I64_INVALID_OPCODE
	},
	[I64_OPCODE_VALUE(gSUB_64_64)] = {
		[FORM_RM_R] = SUB_64_R64,
		[FORM_R_RM] = SUB_R64_64,
		[FORM_RM_IMM] = SUB_AL_IMM8, 
		[FORM_rAX_IMM] = SUB_rAX_IMM32
	},
	[I64_OPCODE_VALUE(gMOV_8_8)] = {
		[FORM_RM_R] = I64_INVALID_OPCODE,
		[FORM_R_RM] = I64_INVALID_OPCODE,
		[FORM_RM_IMM] = I64_INVALID_OPCODE, 
		[FORM_rAX_IMM] = I64_INVALID_OPCODE
	},
	[I64_OPCODE_VALUE(gMOV_64_64)] = {
		[FORM_RM_R] = MOV_64_R64,
		[FORM_R_RM] = MOV_R64_64,
		[FORM_RM_IMM] = I64_INVALID_OPCODE, 
		[FORM_rAX_IMM] = I64_INVALID_OPCODE
	},
};

/*
 	Defines how different 2 first instruction operands
	translate from IR to I64 instruction format.

	This is crucial to ensure correct mapping later on
	into instruction opcodes using I64_IR_GENERIC_TO_SPECIFIC.
*/
enum i64_opcode_form I64_IR_FORM_MAP[4][4] = {
	[IR_OP_REG] = {
		[IR_OP_REG] = FORM_RM_R,
		[IR_OP_MEM] = FORM_R_RM,
		[IR_OP_IMM] = FORM_RM_IMM,
	},
	[IR_OP_MEM] = {
		[IR_OP_REG] = FORM_RM_R,
		[IR_OP_MEM] = FORM_UNK,
		[IR_OP_IMM] = FORM_RM_IMM,
	},
	/*
	 	Impossible case in IA-32e ISA
	*/
	[IR_OP_IMM] = {
		[IR_OP_REG] = FORM_UNK,
		[IR_OP_MEM] = FORM_UNK,
		[IR_OP_IMM] = FORM_UNK,
	},
};

