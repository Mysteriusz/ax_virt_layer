#include "mte/cpu.h"

#include "mips/mips32_ir.h"
#include "intel/intel64_ir.h"

org_to_ir_call arch_org_to_ir(
	_in enum mte_arch 	arch
){
	switch(arch){
	case MIPS32:
		return nullptr;
	case INTEL64:
		return (org_to_ir_call)mips32_to_ir_call;
	default:
		return (org_to_ir_call)_invalid_call;
	}
}
tar_to_ir_call arch_ir_to_tar(
	_in enum mte_arch 	arch
){
	switch(arch){
	case MIPS32:
		return nullptr;
	case INTEL64:
		return (tar_to_ir_call)mips32_to_ir_call;
	default:
		return (tar_to_ir_call)_invalid_call;
	}
}

ir_context ir_init(
	_in const u64 		version,
	_in enum mte_arch 	org_arch,
	_in enum mte_arch 	tar_arch
){
	ir_context buf = (ir_context){
		.rule = (ir_rule){
			.type = IR_RULE_CONTEXT_DESC,
			.data = *(const ir_unk_data*)&(const struct ir_context_desc){
				.org_map = _arch_to_map(org_arch),
				.tar_map = _arch_to_map(tar_arch),
				.org_arch = org_arch,
				.tar_arch = tar_arch,
				.call.org_to_ir = arch_org_to_ir(org_arch),
				.call.ir_to_tar = arch_ir_to_tar(tar_arch),
			},
		},
		.blocked = false,
		.version = version
	};

	return buf;
}


