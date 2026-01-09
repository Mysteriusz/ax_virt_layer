#if !defined(MTE_IR_CORE_INT)
#define MTE_IR_CORE_INT

#include <ax_error.h>
#include <ax_memory.h>
#include <stdatomic.h>

#include "mte/cpu.h"

#define IR_VER		"MTE_IR_0.01"

typedef struct _ir_context{
	const c8 		ver[16]; // Ex: MTE_IR_0.01\0, MTE_IR_123.45\0
	_Atomic u32 		stream_count;
	_Atomic bool 		blocked;
	struct cpu_reg_map 	reg_map;
} ir_context;

typedef struct _ir_reg{
	u64 			val;
	enum : u8{
		IR_REG_KEY, // val == Arch specific key representing this register
		IR_REG_VAL, // val == Any value
	} type;
} ir_reg;

typedef struct _ir_rule{
	enum : u8{
		IR_RULE_OP_INFO, // data* == u32*
		IR_RULE_REG_INFO,
	} const type;
	const void *const 	data;
} ir_rule;

// Initalize IR register on the stack
#define init_ir_reg(v, t) 	((ir_reg){.val = (v), .type = (t)})

typedef enum _ir_opcode : u32{
	ADD_I32 = 0,
	ADD_I32_IMMD = 1
} ir_opcode;

typedef struct _ir_raw_instr{
	ir_context *const 	ir;
	ir_opcode 		opcode;
	u8			reg_used;
	ir_reg			reg[4]; // Each reg corresponds to offset of ir->reg_map
} ir_raw_instr;

// Initialize IR instruction on the stack with context
#define init_ir_raw_instr(irc) 	((ir_raw_instr){.ir = (ir_context*)(irc)})

#endif // !defined(MTE_IR_CORE_INT)

