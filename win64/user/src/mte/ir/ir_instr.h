#if !defined(MTE_IR_INSTR_INT)
#define MTE_IR_INSTR_INT

#include "ir_operand.h"
#include "ir_opcode.h"

// Initialize IR instruction on the stack with context
#define init_ir_raw_instr(irc) 	((ir_raw_instr){.ir = (ir_context*)(irc)})

typedef struct _ir_raw_instr{
	ir_opcode 		opcode;
	ir_operand_set		set;
} ir_raw_instr;

#endif // !defined(MTE_IR_INSTR_INT)

