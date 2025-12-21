#if !defined(MTE_IR_CORE_INT)
#define MTE_IR_CORE_INT

#include <ax_error.h>
#include <ax_memory.h>
#include <stdatomic.h>

#include "mte/cpu.h"

#define IR_VER		"MTE_IR_0.01"

typedef struct _ir_context{
	const c8 		ver[14]; // Ex: MTE_IR_0.01\0, MTE_IR_123.45\0
	_Atomic u32 		stream_count;
	_Atomic bool 		blocked;
	struct cpu_reg_map 	reg_map;
} ir_context;

typedef struct _ir_raw_instr{
	ir_context		*ir;
	u32 			opcode;
	u16			reg[10]; // Each reg corresponds to offset of ir->reg_map
	u8			reg_used;
} ir_raw_instr;

// Initialize ir instruction on the stack for context
#define init_ir_raw_instr(irc) ((ir_raw_instr){.ir = (ir_context*)(irc)})

typedef enum _ir_opcode : u32{
	ADD_I32_IMMD = 1
} ir_opcode;

#endif // !defined(MTE_IR_CORE_INT)

