#if !defined(MTE_IR_INSTR_INT)
#define MTE_IR_INSTR_INT

#include "ir_operand.h"

typedef enum _ir_opcode : u16{
	IR_INVALID_OPCODE = 0,
	IR_ADD_I32 = 1, // reg0 = add i32 reg1, reg2
} ir_opcode;

// Initialize IR instruction on the stack with context
#define init_ir_raw_instr(irc) 	((ir_raw_instr){.ir = (ir_context*)(irc)})

typedef struct _ir_raw_instr{
	ir_opcode 		opcode;
	u8			reg_used;
	ir_operand		regs[4];
} ir_raw_instr;

#endif // !defined(MTE_IR_INSTR_INT)

