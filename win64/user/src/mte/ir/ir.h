#if !defined(MTE_IR_CORE_INT)
#define MTE_IR_CORE_INT

#include <ax_error.h>
#include <ax_memory.h>
#include <stdatomic.h>

#include "mte/arch.h"
#include "mte/cpu.h"
#include "mte/core.h"

#include "ir_instr.h"

#define IR_VER		*(u64*)"0.01"

typedef struct _ir_context ir_context;

typedef u64 ir_unk_data;
/*
 	ir_unk_data overloads
*/

/*
 	Ir context call interface
*/
typedef ir_raw_instr (*const org_to_ir_call)(
	_in mte_raw_instr 	instr,
	_in ir_context 		*context
);
typedef mte_raw_instr (*const ir_to_tar_call)(
	_in ir_raw_instr 	instr,
	_in ir_context 		*context
);

/*
 	IR_RULE_CONTEXT_DESC
*/
struct ir_context_desc{
	enum mte_arch 			org_arch;
	enum mte_arch 			tar_arch;
	struct cpu_reg_map *const org_map;
	struct cpu_reg_map *const	tar_map;
	const struct{
		org_to_ir_call org_to_ir;
		ir_to_tar_call ir_to_tar;
	} call;
};

typedef struct _ir_rule{
	enum : u8{
		IR_RULE_CONTEXT_DESC, // IR operation descriptor (data -> ir_context_desc)
		IR_RULE_OP_DESC, // IR operation descriptor (data -> ir_op_desc)
		IR_RULE_REG_DESC, // IR register descriptor (data -> ir_reg_desc)
	} const type;
	// Type of the data stored depends on the [type] field
	ir_unk_data 	data;
} ir_rule;

#define ir_rule_to_context(r_p) ( \
	(struct ir_context_desc*)((ir_unk_data*)(((ir_rule*)(r_p))->data)) \
)

typedef struct _ir_context{
	const u64 		version; // Ex: 0.01\0, 123.45\0
	_Atomic bool 		blocked;
	ir_rule 		rule; // type == IR_RULE_CONTEXT_DESC
} ir_context;

_inline_avert axres ir_create(
	_in const u64 		version,
	_in enum mte_arch 	org_arch,
	_in enum mte_arch 	tar_arch,
	_out ir_context		**buf
);

_inline_avert void ir_delete(
	_in ir_context 		*ir
);

static void _invalid_call(
	ir_context 	*context
){
	exit(1);
}

org_to_ir_call arch_org_to_ir(
	_in enum mte_arch 	arch
);
ir_to_tar_call arch_ir_to_tar(
	_in enum mte_arch 	arch
);

#endif // !defined(MTE_IR_CORE_INT)

