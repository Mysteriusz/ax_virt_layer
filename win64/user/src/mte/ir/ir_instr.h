#if !defined(MTE_IR_INSTR_INT)
#define MTE_IR_INSTR_INT

#include "mte/ir/ir_operand.h"
#include "mte/ir/ir_opcode.h"

typedef struct _ir_raw_instr{
	ir_opcode 		opcode;
	ir_operand_set		set;
} ir_raw_instr;
static_asrt(divide_compatible(sizeof(ir_raw_instr), 16));

#endif // !defined(MTE_IR_INSTR_INT)

