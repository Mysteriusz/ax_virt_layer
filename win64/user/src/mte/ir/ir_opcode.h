#if !defined(MTE_IR_OPCODE_INT)

#include <ax_type.h>

/*
 	ret -> Return value present (1 or 0)
 	wid -> Operand bit-width encoding (0-3)
 	grp -> Opcode group
*/
#define IR_OPI(ret, wid, grp) \
	(((ret & 1) << 15) | ((wid & 3) << 13) | (grp & 0x1fff))

#define IR_8BIT  0
#define IR_16BIT 1
#define IR_32BIT 2
#define IR_64BIT 3

#define IR_GROUP_ADD 1

typedef enum _ir_opcode : u16{
	IR_INVALID_OPCODE = 0,
	IR_ADD_I64 	= IR_OPI(1, IR_64BIT, IR_GROUP_ADD),	 // op[0] = add i64 op[1], op[2]
	IR_ADD_I32 	= IR_OPI(1, IR_32BIT, IR_GROUP_ADD),	 // op[0] = add i32 op[1], op[2]
	IR_ADD_I16 	= IR_OPI(1, IR_16BIT, IR_GROUP_ADD),	 // op[0] = add i16 op[1], op[2]
	IR_ADD_I8 	= IR_OPI(1, IR_8BIT, IR_GROUP_ADD),	 // op[0] = add i8 op[1], op[2]
} ir_opcode;

#define IR_OPCODE_HAS_RETURN(opcode) 	((opcode >> 15) & 1)
#define IR_OPCODE_WIDTH(opcode) 	((opcode >> 13) & 3)
#define IR_OPCODE_GROUP(opcode) 	(opcode & 0x1fff)

#endif // !defined(MTE_IR_OPCODE_INT)

