#include "mte/cpu.h"

#include "mips/mips32_ir.h"
#include "intel/i64_ir.h"

org_to_ir_call arch_org_to_ir(
	_in enum mte_arch 	arch
){
	switch(arch){
	case MIPS32:
		return (org_to_ir_call)mips32_to_ir_call;
	case INTEL64:
		return (org_to_ir_call)_invalid_call;
	default:
		return (org_to_ir_call)_invalid_call;
	}
}
ir_to_tar_call arch_ir_to_tar(
	_in enum mte_arch 	arch
){
	switch(arch){
	case MIPS32:
		return (ir_to_tar_call)_invalid_call;
	case INTEL64:
		return (ir_to_tar_call)_invalid_call;
	default:
		return (ir_to_tar_call)_invalid_call;
	}
}
role_to_reg_call arch_role_to_reg(
	_in enum mte_arch 	arch
){
	switch(arch){
	case MIPS32:
		return (role_to_reg_call)_invalid_call;
	case INTEL64:
		return (role_to_reg_call)i64_role_to_reg;
	default:
		return (role_to_reg_call)_invalid_call;
	}
}
reg_to_role_call arch_reg_to_role(
	_in enum mte_arch 	arch
){
	switch(arch){
	case MIPS32:
		return (reg_to_role_call)mips32_reg_to_role;
	case INTEL64:
		return (reg_to_role_call)_invalid_call;
	default:
		return (reg_to_role_call)_invalid_call;
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
		// TODO: CPU State instead of org_map
		/*.org_map = _arch_to_map(org_arch),
		.tar_map = _arch_to_map(tar_arch),*/
		.org_arch = org_arch,
		.tar_arch = tar_arch,
		.call = {
			.org_to_ir = arch_org_to_ir(org_arch),
			.ir_to_tar = arch_ir_to_tar(tar_arch),

			.org_role_to_reg = arch_role_to_reg(org_arch),
			.tar_role_to_reg = arch_role_to_reg(tar_arch),

			.org_reg_to_role = arch_reg_to_role(org_arch),
			.tar_reg_to_role = arch_reg_to_role(tar_arch),
		},
	};
	struct ir_context_desc *desc =
		axmalloc(sizeof(struct ir_context_desc));
	memcpy(desc, &temp_desc, sizeof(struct ir_context_desc));

	// Create temporary IR context
	ir_context temp_ir = (ir_context){
		.rule.type = IR_RULE_CONTEXT_DESC,
		.rule.data = (ir_unk_data)desc,
		.blocked = false,
		.version = version,
	};
	ir_context *ir =
		axmalloc(sizeof(ir_context));
	memcpy(ir, &temp_ir, sizeof(ir_context));

	if ((u64)desc->call.org_to_ir != (u64)_invalid_call){
		desc->call.org_to_ir((mte_raw_instr){0}, nullptr);
	}
	if ((u64)desc->call.ir_to_tar != (u64)_invalid_call){
		desc->call.ir_to_tar((ir_raw_instr){0}, nullptr);
	}
	if ((u64)desc->call.org_role_to_reg != (u64)_invalid_call){
		desc->call.org_role_to_reg(0);
	}
	if ((u64)desc->call.tar_role_to_reg != (u64)_invalid_call){
		desc->call.tar_role_to_reg(0);
	}
	if ((u64)desc->call.org_reg_to_role != (u64)_invalid_call){
		desc->call.org_reg_to_role(0);
	}
	if ((u64)desc->call.tar_reg_to_role != (u64)_invalid_call){
		desc->call.tar_reg_to_role(0);
	}

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

