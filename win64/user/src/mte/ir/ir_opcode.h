#if !defined(MTE_IR_OPCODE_INT)

#include <ax_type.h>

#define IR_DEST_SRC_ACC BIT(13)
/*
 	ret -> Return value present (1 bit -> 0-1)
 	wid -> Operand bit-width encoding (2 bits -> 0-3)

	Flag field is an inherit part of the group
 	flg -> (3 bit flags) 
		- BIT 13 -> IR_DEST_SRC_ACC
		- BIT 12 -> RESERVED
		- BIT 11 -> RESERVED
 	grp -> Opcode group (10 bits -> 0-1023)
*/
#define IR_OPI(ret, wid, grp) \
	(((ret & 1) << 15) | ((wid & 3) << 13) | (grp & 0x1fff))

#define IR_8BIT  0
#define IR_16BIT 1
#define IR_32BIT 2
#define IR_64BIT 3

/*
 	Opcode groups identify the type of instruction
	and it`s semantics for all instructions in that group
*/
#define IR_GROUP_ADD 1
#define IR_GROUP_MOV (IR_DEST_SRC_ACC | 2)

typedef enum _ir_opcode : u16{
	IR_INVALID_OPCODE = 0,

	IR_ADD_I64 	= IR_OPI(1, IR_64BIT, IR_GROUP_ADD),	 // op[0] = add i64 op[1], op[2]
	IR_ADD_I32 	= IR_OPI(1, IR_32BIT, IR_GROUP_ADD),	 // op[0] = add i32 op[1], op[2]
	IR_ADD_I16 	= IR_OPI(1, IR_16BIT, IR_GROUP_ADD),	 // op[0] = add i16 op[1], op[2]
	IR_ADD_I8 	= IR_OPI(1, IR_8BIT, IR_GROUP_ADD),	 // op[0] = add i8 op[1], op[2]

	IR_MOV_I64	= IR_OPI(1, IR_64BIT, IR_GROUP_MOV),	 // op[0] = i64 op[1]
	IR_MOV_I32	= IR_OPI(1, IR_32BIT, IR_GROUP_MOV),	 // op[0] = i32 op[1]
	IR_MOV_I16	= IR_OPI(1, IR_16BIT, IR_GROUP_MOV),	 // op[0] = i16 op[1]
	IR_MOV_I8	= IR_OPI(1, IR_8BIT, IR_GROUP_MOV),	 // op[0] = i8 op[1]
} ir_opcode;

#define IR_OPCODE_HAS_RETURN(opcode) 	((opcode >> 15) & 1)
#define IR_OPCODE_WIDTH(opcode) 	((opcode >> 13) & 3)
#define IR_OPCODE_FLAGS(opcode) 	((opcode >> 10) & 7)
#define IR_OPCODE_GROUP(opcode) 	(opcode & 0x3ff)
#define IR_OPCODE_GROUP_NO_FLAG(opcode) (opcode & 0x3ff)
#define ir_opcode_swap_group(opcode, group) 	((opcode & ~0x1fff) | group)

#endif // !defined(MTE_IR_OPCODE_INT)

