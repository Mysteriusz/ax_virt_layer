#if !defined(MTE_I64_OPCODE_INT)
#define MTE_I64_OPCODE_INT

#include <ax_type.h>

#include "i64_operand.h"

// Maximum byte length of an opcode
#define I64_MAX_OPCODE_LEN 	4

// Initialize opcode length (l) and value (v) (shouldn`t be used externaly)
#define I64_OI(l, v) \
	((l##ULL << 56) | (v))

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
	ADD_8_R8 	= I64_OI(1, 0x00), 	// ADD	r/m8		r8
	ADD_64_R64 	= I64_OI(1, 0x01), 	// ADD	r/m16/32/64	r16/32/64
	ADD_R8_8 	= I64_OI(1, 0x02), 	// ADD	r8		r/m8
	ADD_R64_64 	= I64_OI(1, 0x03),	// ADD	r16/32/64	r/m16/32/64
	ADD_AL_IMM8 	= I64_OI(1, 0x04), 	// ADD	AL		imm8
	ADD_AX_IMM32 	= I64_OI(1, 0x05), 	// ADD	rAX		imm16/32
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
	i64_operand_desc 		desc[I64_RED_OP_COUNT];
	u8 				op_count : 4;
	enum i64_opcode_flags 		flags : 4;
	enum i64_opcode_prefix 		prefix;
} i64_opcode_desc;

/*
 	Table of metadata structs of only 1 byte length opcode
*/
extern i64_opcode_desc L1_OPCODE_META_TABLE[0xff];

#endif // !defined(MTE_I64_OPCODE_INT)

