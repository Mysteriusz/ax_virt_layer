#include "mte/cpu.h"

#include "mips/mips32_ir.h"

#include "intel/i64_ir.h"
#include "intel/i64_cpu.h"

static struct cpu_reg_map *_arch_to_map(
	_in enum mte_arch 	arch
){
	switch(arch){
	case MIPS32:
		return &_MIPS32_CPU_REG_MAP;
	case INTEL64:
		i64_load_cpu_reg_map();
		return &_I64_CPU_REG_MAP;
	default:
		return nullptr;
	}
}
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
		return (ir_to_tar_call)i64_ir_to_raw;
	default:
		return (ir_to_tar_call)_invalid_call;
	}
}
org_reg_fetch_call arch_org_reg_fetch(
	_in enum mte_arch 	arch
){
	switch(arch){
	case MIPS32:
		return (org_reg_fetch_call)mips32_reg_fetch_call;
	case INTEL64:
		return (org_reg_fetch_call)_invalid_call;
	default:
		return (org_reg_fetch_call)_invalid_call;
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

	// Create temporary IR context
	ir_context temp_ir = (ir_context){
		.desc = {
			.org_map = _arch_to_map(org_arch),
			.tar_map = _arch_to_map(tar_arch),
			.org_arch = org_arch,
			.tar_arch = tar_arch,
			.call = {
				.org_to_ir = arch_org_to_ir(org_arch),
				.ir_to_tar = arch_ir_to_tar(tar_arch),
				.org_reg_fetch = arch_org_reg_fetch(org_arch),
			},
		},
		.blocked = false,
		.version = version,
	};
	ir_context *ir =
		axmalloc(sizeof(ir_context));
	memcpy(ir, &temp_ir, sizeof(ir_context));

	// Prefetch by calling each function
	if ((u64)ir->desc.call.org_to_ir != (u64)_invalid_call){
		ir->desc.call.org_to_ir((mte_raw_instr){0}, nullptr, nullptr);
	}
	if ((u64)ir->desc.call.ir_to_tar != (u64)_invalid_call){
		ir->desc.call.ir_to_tar((ir_raw_instr){0}, nullptr, nullptr);
	}

	if ((u64)ir->desc.call.org_reg_fetch != (u64)_invalid_call){
		ir->desc.call.org_reg_fetch((mte_raw_instr){0}, nullptr, nullptr);
	}
	*buf = ir;

	return AX_SUCC;
}

_inline_avert void ir_delete(
	_in ir_context 		*ir
){
	if (ir){
		axfree(ir);
	}
}

