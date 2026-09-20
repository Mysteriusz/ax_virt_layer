#if !defined(MTE_IR_OPCODE_INT)

#include <ax_type.h>

/*
 	Destination does not equal source.
	This flag is specifically targeted towards ISA`s which
	require dest/src operand 'MTE_ISA_TWO_OP'

	This allows to override checks for dest/src operands
	in the ISA`s IR implementations

	NOTICE:
	This may become deprecated in the future
*/
#define IR_DEST_NEQ_SRC BIT(2)

/*
 	ret -> Return value present (1 bit -> 0-1)
 	wid -> Operand bit-width encoding (2 bits -> 0-3)

	Flag field is an inherit part of the group
 	flg -> (3 bit flags) 
		- BIT 13 -> IR_DEST_NEQ_SRC
		- BIT 12 -> RESERVED
		- BIT 11 -> RESERVED
 	grp -> Opcode group (10 bits -> 0-1023)
*/
#define _IR_OPI(ret, wid, grp) \
	(((ret & 1) << 15) | ((wid & 3) << 13) | (grp & 0x1fff))

/*
	Opcode width identify the bit-width of the instruction
*/

#define IR_8BIT  0
#define IR_16BIT 1
#define IR_32BIT 2
#define IR_64BIT 3

/*
 	Opcode groups identify the type of instruction
	and it`s semantics for all instructions in that group
*/

#define IR_GROUP_INVALID 0
#define IR_GROUP_ADD 1
#define IR_GROUP_OR 2
#define IR_GROUP_AND 3
#define IR_GROUP_SUB 4
#define IR_GROUP_MOV ((IR_DEST_NEQ_SRC << 10) | 5)

typedef enum _ir_opcode : u16{
	IR_INVALID_OPCODE = 0,

	IR_ADD_I64 	= _IR_OPI(1, IR_64BIT, IR_GROUP_ADD),	 // op[0] = add i64 op[1], op[2]
	IR_ADD_I32 	= _IR_OPI(1, IR_32BIT, IR_GROUP_ADD),	 // op[0] = add i32 op[1], op[2]
	IR_ADD_I16 	= _IR_OPI(1, IR_16BIT, IR_GROUP_ADD),	 // op[0] = add i16 op[1], op[2]
	IR_ADD_I8 	= _IR_OPI(1, IR_8BIT, IR_GROUP_ADD),	 // op[0] = add i8 op[1], op[2]

	IR_OR_I64	= _IR_OPI(1, IR_64BIT, IR_GROUP_OR),	 // op[0] = or i64 op[1], op[2]
	IR_OR_I32	= _IR_OPI(1, IR_32BIT, IR_GROUP_OR),	 // op[0] = or i32 op[1], op[2]
	IR_OR_I16	= _IR_OPI(1, IR_16BIT, IR_GROUP_OR),	 // op[0] = or i16 op[1], op[2]
	IR_OR_I8	= _IR_OPI(1, IR_8BIT, IR_GROUP_OR),	 // op[0] = or i8 op[1], op[2]

	IR_AND_I64	= _IR_OPI(1, IR_64BIT, IR_GROUP_AND),	 // op[0] = and i64 op[1], op[2]
	IR_AND_I32	= _IR_OPI(1, IR_32BIT, IR_GROUP_AND),	 // op[0] = and i32 op[1], op[2]
	IR_AND_I16	= _IR_OPI(1, IR_16BIT, IR_GROUP_AND),	 // op[0] = and i16 op[1], op[2]
	IR_AND_I8	= _IR_OPI(1, IR_8BIT, IR_GROUP_AND),	 // op[0] = and i8 op[1], op[2]

	IR_SUB_I64	= _IR_OPI(1, IR_64BIT, IR_GROUP_SUB),	 // op[0] = sub i64 op[1], op[2]
	IR_SUB_I32	= _IR_OPI(1, IR_32BIT, IR_GROUP_SUB),	 // op[0] = sub i32 op[1], op[2]
	IR_SUB_I16	= _IR_OPI(1, IR_16BIT, IR_GROUP_SUB),	 // op[0] = sub i16 op[1], op[2]
	IR_SUB_I8	= _IR_OPI(1, IR_8BIT, IR_GROUP_SUB),	 // op[0] = sub i8 op[1], op[2]

	IR_MOV_I64	= _IR_OPI(1, IR_64BIT, IR_GROUP_MOV),	 // op[0] = mov i64 op[1]
	IR_MOV_I32	= _IR_OPI(1, IR_32BIT, IR_GROUP_MOV),	 // op[0] = mov i32 op[1]
	IR_MOV_I16	= _IR_OPI(1, IR_16BIT, IR_GROUP_MOV),	 // op[0] = mov i16 op[1]
	IR_MOV_I8	= _IR_OPI(1, IR_8BIT, IR_GROUP_MOV),	 // op[0] = mov i8 op[1]
} ir_opcode;

#define IR_OPCODE_HAS_RETURN(opcode) 	((opcode >> 15) & 1)
#define IR_OPCODE_WIDTH(opcode) 	((opcode >> 13) & 3)
#define IR_OPCODE_FLAGS(opcode) 	((opcode >> 10) & 7)
#define IR_OPCODE_GROUP(opcode) 	(opcode & 0x3ff)

#define ir_opcode_swap_group(opcode, group) 	((opcode & ~0x1fff) | group)

#endif // !defined(MTE_IR_OPCODE_INT)

