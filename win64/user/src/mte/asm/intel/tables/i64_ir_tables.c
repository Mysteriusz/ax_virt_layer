#include "intel/i64_ir.h"

/*
 	Each group requires generics for each bit-width in order:
	IR_8BIT
	IR_16BIT
	IR_32BIT
	IR_64BIT
*/
enum i64_opcode I64_IR_GROUP_TO_GENERIC[][8] = {
	[IR_OPCODE_GROUP_NO_FLAG(IR_GROUP_ADD)] = {
		[IR_8BIT] = gADD_8_8,
		[IR_16BIT] = gADD_64_64,
		[IR_32BIT] = gADD_64_64, 
		[IR_64BIT] = gADD_64_64
	},
	[IR_OPCODE_GROUP_NO_FLAG(IR_GROUP_MOV)] = {
		[IR_8BIT] = gMOV_8_8,
		[IR_16BIT] = gMOV_64_64,
		[IR_32BIT] = gMOV_64_64,
		[IR_64BIT] = gMOV_64_64,
	},
};

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

