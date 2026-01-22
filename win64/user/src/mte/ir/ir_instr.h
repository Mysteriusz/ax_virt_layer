#if !defined(MTE_IR_INSTR_INT)
#define MTE_IR_INSTR_INT

#include "ir_op.h"
#include "ir_reg.h"

// Initialize IR instruction on the stack with context
#define init_ir_raw_instr(irc) 	((ir_raw_instr){.ir = (ir_context*)(irc)})

typedef struct _ir_raw_instr{ _align(16)
	ir_opcode 		opcode;
	u8			reg_used;
	ir_reg_desc		reg[4]; // Each reg corresponds to offset of ir->reg_map
} ir_raw_instr;

#endif // !defined(MTE_IR_INSTR_INT)

