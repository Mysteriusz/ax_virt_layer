#if !defined(MTE_IR_REG_INT)
#define MTE_IR_REG_INT

#include <ax_type.h>

typedef enum _ir_reg_type : u8{
	IR_REG_KEY, // val == Arch specific key representing this register
	IR_REG_VAL, // val == Any value
} ir_reg_type;

typedef struct _ir_reg_desc{
	u64 			id;
} ir_reg_desc;

// Initalize IR register on the stack
#define init_ir_reg(v) 	((ir_reg_desc){.id = (v)})

#endif // !defined(MTE_IR_REG_INT)

