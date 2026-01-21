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

typedef u64 ir_unk_data;
typedef struct _ir_rule{
	enum : u8{
		IR_RULE_CONTEXT_DESC, // IR operation descriptor (data -> ir_context_desc)
		IR_RULE_OP_DESC, // IR operation descriptor (data -> ir_op_desc)
		IR_RULE_REG_DESC, // IR register descriptor (data -> ir_reg_desc)
	} const type;
	// Type of the data stored depends on the [type] field
	const ir_unk_data 	data;
} ir_rule;

typedef struct _ir_context{
	const c8 		ver[16]; // Ex: MTE_IR_0.01\0, MTE_IR_123.45\0
	_Atomic u32 		stream_count;
	_Atomic bool 		blocked;
	struct cpu_reg_map 	reg_map;
	ir_rule 		rule; // type == IR_RULE_CONTEXT_DESC
} ir_context;


#endif // !defined(MTE_IR_CORE_INT)

