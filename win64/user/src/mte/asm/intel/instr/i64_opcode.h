#if !defined(MTE_I64_OPCODE_INT)
#define MTE_I64_OPCODE_INT

#include <ax_type.h>
#include <ax_io.h>

#include "i64_operand.h"

// Maximum byte length of an opcode
#define I64_MAX_OPCODE_LEN 	4

/*
 	len -> Opcode length in bytes
	type -> Type of the encoded opcode (1 -> Generic; 2 -> Specific)
	val -> Value of the opcode
*/
#define _I64_OPI(len, type, val) \
	(((len##ULL & 0xf) << 60) | ((type & 0xf) << 56) | (val & 0xffffffffffffff))

#define I64_OP_GENERIC 1ULL
#define I64_OP_INSTR 2ULL

/*
   	Reference for opcodes used:
		- http://ref.x86asm.net/coder64.html
*/
enum i64_opcode : u64{
	I64_INVALID_OPCODE 	= _I64_OPI(0, 0, 0),

	/*
	 	ADD instruction group (0x00 - 0x05)
	*/

	// GENERICS
	gADD_8_8 	= _I64_OPI(0, I64_OP_GENERIC, 0x00), 	// ADD 	r/m/imm8	r/m/imm8
	gADD_64_64 	= _I64_OPI(0, I64_OP_GENERIC, 0x01), 	// ADD 	r/m/imm64	r/m/imm64

	// OPCODES
	ADD_8_R8 	= _I64_OPI(1, I64_OP_INSTR, 0x00), 	// ADD	r/m8		r8
	ADD_64_R64 	= _I64_OPI(1, I64_OP_INSTR, 0x01), 	// ADD	r/m16/32/64	r16/32/64
	ADD_R8_8 	= _I64_OPI(1, I64_OP_INSTR, 0x02), 	// ADD	r8		r/m8
	ADD_R64_64 	= _I64_OPI(1, I64_OP_INSTR, 0x03),	// ADD	r16/32/64	r/m16/32/64
	ADD_AL_IMM8 	= _I64_OPI(1, I64_OP_INSTR, 0x04), 	// ADD	AL		imm8
	ADD_rAX_IMM32 	= _I64_OPI(1, I64_OP_INSTR, 0x05), 	// ADD	rAX		imm16/32

	/*
	 	OR instruction group (0x08 - 0x0d)
	*/

	// GENERICS
	gOR_8_8 	= _I64_OPI(0, I64_OP_GENERIC, 0x08), 	// OR 	r/m/imm8	r/m/imm8
	gOR_64_64 	= _I64_OPI(0, I64_OP_GENERIC, 0x09), 	// OR 	r/m/imm64	r/m/imm64

	// OPCODES
	OR_8_R8 	= _I64_OPI(1, I64_OP_INSTR, 0x08), 	// OR	r/m8		r8
	OR_64_R64 	= _I64_OPI(1, I64_OP_INSTR, 0x09), 	// OR	r/m16/32/64	r16/32/64
	OR_R8_8 	= _I64_OPI(1, I64_OP_INSTR, 0x0a), 	// OR	r8		r/m8
	OR_R64_64 	= _I64_OPI(1, I64_OP_INSTR, 0x0b),	// OR	r16/32/64	r/m16/32/64
	OR_AL_IMM8 	= _I64_OPI(1, I64_OP_INSTR, 0x0c), 	// OR	AL		imm8
	OR_rAX_IMM32 	= _I64_OPI(1, I64_OP_INSTR, 0x0d), 	// OR	rAX		imm16/32

	/*
	 	AND instruction group (0x20 - 0x25)
	*/

	// GENERICS
	gAND_8_8 	= _I64_OPI(0, I64_OP_GENERIC, 0x20), 	// AND 	r/m/imm8	r/m/imm8
	gAND_64_64 	= _I64_OPI(0, I64_OP_GENERIC, 0x21), 	// AND 	r/m/imm64	r/m/imm64

	// OPCODES
	AND_8_R8 	= _I64_OPI(1, I64_OP_INSTR, 0x20), 	// AND	r/m8		r8
	AND_64_R64 	= _I64_OPI(1, I64_OP_INSTR, 0x21), 	// AND	r/m16/32/64	r16/32/64
	AND_R8_8 	= _I64_OPI(1, I64_OP_INSTR, 0x22), 	// AND	r8		r/m8
	AND_R64_64 	= _I64_OPI(1, I64_OP_INSTR, 0x23),	// AND	r16/32/64	r/m16/32/64
	AND_AL_IMM8 	= _I64_OPI(1, I64_OP_INSTR, 0x24), 	// AND	AL		imm8
	AND_rAX_IMM32 	= _I64_OPI(1, I64_OP_INSTR, 0x25), 	// AND	rAX		imm16/32

	/*
	 	SUB instruction group (0x28 - 0x2d)
	*/

	// GENERICS
	gSUB_8_8 	= _I64_OPI(0, I64_OP_GENERIC, 0x28), 	// SUB 	r/m/imm8	r/m/imm8
	gSUB_64_64 	= _I64_OPI(0, I64_OP_GENERIC, 0x2d), 	// SUB 	r/m/imm64	r/m/imm64

	// OPCODES
	SUB_8_R8 	= _I64_OPI(1, I64_OP_INSTR, 0x28), 	// SUB	r/m8		r8
	SUB_64_R64 	= _I64_OPI(1, I64_OP_INSTR, 0x29), 	// SUB	r/m16/32/64	r16/32/64
	SUB_R8_8 	= _I64_OPI(1, I64_OP_INSTR, 0x2a), 	// SUB	r8		r/m8
	SUB_R64_64 	= _I64_OPI(1, I64_OP_INSTR, 0x2b),	// SUB	r16/32/64	r/m16/32/64
	SUB_AL_IMM8 	= _I64_OPI(1, I64_OP_INSTR, 0x2c), 	// SUB	AL		imm8
	SUB_rAX_IMM32 	= _I64_OPI(1, I64_OP_INSTR, 0x2d), 	// SUB	rAX		imm16/32

	/*
	 	PUSH instruction group (0x50)
	*/

	// GENERICS
	gPUSH_64 	= _I64_OPI(0, I64_OP_GENERIC, 0x50), 	// PUSH r/m/imm64

	// OPCODES
	PUSH_R64 	= _I64_OPI(1, I64_OP_INSTR, 0x50), 	// PUSH r64/16

	/*
	 	MOV instruction group (0x88 - 0x8b)
	*/

	// GENERICS
	gMOV_8_8 	= _I64_OPI(0, I64_OP_GENERIC, 0x88), 	// MOV 	r/m/imm8	r/m/imm8
	gMOV_64_64 	= _I64_OPI(0, I64_OP_GENERIC, 0x89), 	// MOV 	r/m/imm64	r/m/imm64

	// OPCODES
	MOV_8_R8 	= _I64_OPI(1, I64_OP_INSTR, 0x88), 	// MOV	r/m8		r8
	MOV_64_R64 	= _I64_OPI(1, I64_OP_INSTR, 0x89), 	// MOV	r/m16/32/64	r16/32/64
	MOV_R8_8 	= _I64_OPI(1, I64_OP_INSTR, 0x8a), 	// MOV	r8		r/m8
	MOV_R64_64 	= _I64_OPI(1, I64_OP_INSTR, 0x8b), 	// MOV	r16/32/64	r/m16/32/64
};

// Read lower 32 bits (unique opcode/generic value)
#define I64_OPCODE_VALUE(opcode) (opcode & ~0UL)
// Read last byte`s lower 4 bits (type)
#define I64_OPCODE_TYPE(opcode) ((opcode >> 56) & 0xf)
// Read last byte`s upper 4 bits (length)
#define I64_OPCODE_LEN(opcode) ((opcode >> 60) & 0xf)

/*
 	Notice:
 	Form count should be capped to 15

	(f) -> Destination and source register are swapped
	(i) -> Unique ID of the form
*/
#define _I64_FORM_I(f, i) \
	((((f) & 1) << 3) | ((i) & 0xf))

/*
 	Forms define which and how instruction
	arguments are written
*/
enum i64_opcode_form : u8{
	FORM_RM_R 	= _I64_FORM_I(1, 0), 	// r/m 		r
	FORM_R_RM 	= _I64_FORM_I(0, 1), 	// r 		r/m
	FORM_RM_IMM 	= _I64_FORM_I(1, 2), 	// r/m 		imm
	FORM_rAX_IMM 	= _I64_FORM_I(0, 3), 	// raX 		imm

	FORM_R 		= _I64_FORM_I(0, 4), 	// r

	FORM_UNK 	= _I64_FORM_I(0, 0), 	// r/m/imm 	r/m/imm
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

typedef struct _i64_opcode_desc{ _align(16)
	i64_operand_desc 		ops[I64_MAX_OP_COUNT];
	u8 				ops_count;
	enum i64_opcode_flags 		flags;
	enum i64_opcode_prefix 		prefix;
	enum i64_opcode_form 		form;
} i64_opcode_desc;

/*
 	Table of metadata structs of only 1 byte length opcode
*/
extern i64_opcode_desc L1_OPCODE_META_TABLE[0xff];

_inline_force i64_opcode_desc _i64_lookup_opcode_meta(
	_in enum i64_opcode	opcode
){
	// Read encoded opcode length (defined by _I64_OPI)
	u8 opcode_len = I64_OPCODE_LEN(opcode);

	if (__builtin_expect(opcode_len > I64_MAX_OPCODE_LEN, false)){
		return (i64_opcode_desc){0};
	}

	// Map [opcode] to opcode desc table based on length
	switch(opcode_len){
	case 1:
		return L1_OPCODE_META_TABLE[opcode & 0xff];
	default:
		return (i64_opcode_desc){0};
	}
}

#endif // !defined(MTE_I64_OPCODE_INT)

