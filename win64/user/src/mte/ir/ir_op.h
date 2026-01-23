#if !defined(MTE_IR_OP_INT)
#define MTE_IR_OP_INT

#include <ax_type.h>

typedef enum _ir_opcode : u32{
	IR_INVALID_OPCODE = 0,
	ADD_I32 = 1,
	ADD_I32_IMMD = 2
} ir_opcode;


#endif // !defined(MTE_IR_OP_INT)

