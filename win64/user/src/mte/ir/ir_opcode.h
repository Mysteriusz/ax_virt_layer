#if !defined(MTE_IR_OPCODE_INT)

#include <ax_type.h>

typedef enum _ir_opcode : u16{
	IR_INVALID_OPCODE = 0,
	IR_ADD_I32 = 1,	 		// op[0] = add i32 op[1], op[2]
} ir_opcode;

#endif // !defined(MTE_IR_OPCODE_INT)

