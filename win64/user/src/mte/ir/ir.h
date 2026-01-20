#if !defined(MTE_IR_CORE_INT)
#define MTE_IR_CORE_INT

#include <ax_error.h>
#include <ax_memory.h>
#include <stdatomic.h>

#include "mte/cpu.h"

#include "ir_op.h"
#include "ir_reg.h"
#include "ir_instr.h"

#define IR_VER		"MTE_IR_0.01"

typedef struct _ir_context{
	const c8 		ver[16]; // Ex: MTE_IR_0.01\0, MTE_IR_123.45\0
	_Atomic u32 		stream_count;
	_Atomic bool 		blocked;
	struct cpu_reg_map 	reg_map;
} ir_context;

typedef u64 ir_unk_data;
typedef struct _ir_rule{
	enum : u8{
		IR_RULE_OP_INFO, // IR operation descriptor
		IR_RULE_REG_INFO, // IR register descriptor
	} const type;
	const ir_unk_data 	data;
} ir_rule;

#endif // !defined(MTE_IR_CORE_INT)

