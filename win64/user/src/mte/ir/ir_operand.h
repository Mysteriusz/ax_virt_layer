#if !defined(MTE_IR_OPERAND_INT)
#define MTE_IR_OPERAND_INT

#include <ax_type.h>

typedef enum _ir_operand_id : u8{
	IR_OP_REG, // [value] == arch specific register id
	IR_OP_MEM, // [value] == index in cpu pointer buffer
	IR_OP_IMM, // [value] == index in cpu immediate buffer
} ir_operand_id;

typedef struct _ir_operand{
	ir_operand_id 		id;
	u8 			value;
} ir_operand;

#endif // !defined(MTE_IR_OPERAND_INT)

