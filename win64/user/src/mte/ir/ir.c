#include "mte/cpu.h"

#include "mips/mips32_ir.h"
#include "intel/intel64_ir.h"

org_to_ir_call arch_org_to_ir(
	_in enum mte_arch 	arch
){
	switch(arch){
	case MIPS32:
		return (org_to_ir_call)mips32_to_ir_call;
	case INTEL64:
		return (org_to_ir_call)mips32_to_ir_call;
	default:
		return (org_to_ir_call)_invalid_call;
	}
}
ir_to_tar_call arch_ir_to_tar(
	_in enum mte_arch 	arch
){
	switch(arch){
	case MIPS32:
		return (ir_to_tar_call)mips32_to_ir_call;
	case INTEL64:
		return (ir_to_tar_call)mips32_to_ir_call;
	default:
		return (ir_to_tar_call)_invalid_call;
	}
}

_inline_avert axres ir_create(
	_in const u64 		version,
	_in enum mte_arch 	org_arch,
	_in enum mte_arch 	tar_arch,
	_out ir_context		**buf
){
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	/*
	 	Controlled undefined behaviour (const overwrite)
	*/

	// Create temporary IR context descriptor
	struct ir_context_desc temp_desc = (struct ir_context_desc){
		.org_map = _arch_to_map(org_arch),
		.tar_map = _arch_to_map(tar_arch),
		.org_arch = org_arch,
		.tar_arch = tar_arch,
		.call = {
			.org_to_ir = arch_org_to_ir(org_arch),
			.ir_to_tar = arch_ir_to_tar(tar_arch),
		},
	};
	struct ir_context_desc *desc
		= axmalloc(sizeof(struct ir_context_desc));
	memcpy(desc, &temp_desc, sizeof(struct ir_context_desc));

	// Create temporary IR context
	ir_context temp_ir = (ir_context){
		.rule.type = IR_RULE_CONTEXT_DESC,
		.rule.data = (ir_unk_data)desc,
		.blocked = false,
		.version = version,
	};
	ir_context *ir
		= axmalloc(sizeof(ir_context));
	memcpy(ir, &temp_ir, sizeof(ir_context));

	*buf = ir;

	return AX_SUCC;
}

_inline_avert void ir_delete(
	_in ir_context 		*ir
){
	if (ir){
		if (ir->rule.data){
			axfree((void*)ir->rule.data);
		}
		axfree(ir);
	}
}

