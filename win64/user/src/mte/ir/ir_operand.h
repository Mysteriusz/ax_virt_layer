#if !defined(MTE_IR_OPERAND_INT)
#define MTE_IR_OPERAND_INT

#include <ax_type.h>

#include "ir_reg.h"

typedef enum _ir_operand_id : u8{
	IR_OP_REG, // [val] == arch specific register id
	IR_OP_MEM, // [val] == index in cpu pointer buffer
	IR_OP_IMM, // [val] == index in cpu immediate buffer
} ir_operand_id;

typedef struct _ir_operand{
	ir_operand_id 		id;
	u8 			val;
} ir_operand;

#endif // !defined(MTE_IR_OPERAND_INT)

