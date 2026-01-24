#if !defined(MTE_IR_OP_INT)
#define MTE_IR_OP_INT

#include <ax_type.h>

typedef enum _ir_opcode : u16{
	IR_INVALID_OPCODE = 0,
	IR_ADD_I32 = 1,
} ir_opcode;


#endif // !defined(MTE_IR_OP_INT)

