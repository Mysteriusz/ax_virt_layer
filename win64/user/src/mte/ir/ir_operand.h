#if !defined(MTE_IR_OPERAND_INT)
#define MTE_IR_OPERAND_INT

#include <ax_type.h>

typedef enum _ir_operand_id : u8{
	IR_OP_REG = 0, // [value] == arch specific register id
	IR_OP_MEM = 1, // [value] == index in ir pointer buffer
	IR_OP_IMM = 2, // [value] == index in ir imimediate buffer
} ir_operand_id;

typedef struct _ir_operand{
	ir_operand_id 		id;
	u8 			value;
} ir_operand;

typedef struct _ir_operand_set{
	ir_operand 	ops[6];
	u8 		ops_count; 	// Used [ops] count
} ir_operand_set;

#endif // !defined(MTE_IR_OPERAND_INT)

