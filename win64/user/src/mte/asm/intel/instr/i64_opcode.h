#if !defined(MTE_I64_OPCODE_INT)
#define MTE_I64_OPCODE_INT

#include <ax_type.h>

#include "i64_operand.h"

// Maximum byte length of an opcode
#define I64_MAX_OPCODE_LEN 	4

/*
 	len -> Opcode length in bytes
	type -> Type of the encoded opcode (1 -> Generic; 2 -> Specific)
	val -> Value of the opcode
*/
#define I64_OI(len, type, val) \
	(((len##ULL & 0xf) << 60) | ((type##ULL & 0xf) << 56) | (val & 0xffffffffffffff))

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
enum i64_opcode : u64{
	I64_INVALID_OPCODE 	= I64_OI(0, 0, 0),
	/*
	 	ADD instruction group
	*/
	// GENERICS
	ADD_8_8 	= I64_OI(0, 1, 0x00), 	// ADD 	r/m/imm8	r/m/imm8
	ADD_64_64 	= I64_OI(0, 1, 0x01), 	// ADD 	r/m/imm8	r/m/imm8
	// OPCODES
	ADD_8_R8 	= I64_OI(1, 2, 0x00), 	// ADD	r/m8		r8
	ADD_64_R64 	= I64_OI(1, 2, 0x01), 	// ADD	r/m16/32/64	r16/32/64
	ADD_R8_8 	= I64_OI(1, 2, 0x02), 	// ADD	r8		r/m8
	ADD_R64_64 	= I64_OI(1, 2, 0x03),	// ADD	r16/32/64	r/m16/32/64
	ADD_AL_IMM8 	= I64_OI(1, 2, 0x04), 	// ADD	AL		imm8
	ADD_rAX_IMM32 	= I64_OI(1, 2, 0x05), 	// ADD	rAX		imm16/32
};
#define I64_OPCODE_VALUE(opcode) (opcode & ~0UL)

/*
 	Notice:
 	Form count should be capped to 15
*/
enum i64_opcode_form : u8{
	FORM_RM_R = 0, 		// r/m 		r
	FORM_R_RM = 1, 		// r 		r/m
	FORM_RM_IMM = 2, 	// r/m 		imm
	FORM_rAX_IMM = 3, 	// raX 		imm
	FORM_UNK = 0, 		// r/m/imm 	r/m/imm
};

enum i64_opcode_flags : u8{
	REX 	= 0x01,
	MODRM 	= 0x02,
	LEGACY 	= 0x04,
	/*	
	LOCK 	= 0x08 | LEGACY,
	PUSH 	= 0x08 | LEGACY,
	POP 	= 0x08 | LEGACY,
	DPUSH 	= 0x08 | LEGACY,*/
};

/*
	Mandatory prefix for opcodes
*/
enum i64_opcode_prefix : u8{
	I64_NO_PREF 	= 0x00,
	I64_LOCK_PREF 	= 0xf0,
	/*
		87x FPU Specific
	*/
	I64_PUSH_PREF 	= 0xd9,
	I64_POP_PREF 	= 0xd9,
	I64_DPUSH_PREF 	= 0xda,
};

/*
	FOR SIMD THERE SHOULD BE A SEPARATE META
*/ 
typedef struct _i64_opcode_desc{ _align(2)
	i64_operand_desc 		ops[I64_RED_OP_COUNT];
	u8 				op_count : 4;
	enum i64_opcode_flags 		flags : 4;
	enum i64_opcode_prefix 		prefix;
} i64_opcode_desc;

/*
 	Table of metadata structs of only 1 byte length opcode
*/
extern i64_opcode_desc L1_OPCODE_META_TABLE[0xff];

#endif // !defined(MTE_I64_OPCODE_INT)

