#include "mte/cpu.h"

#include "mips/mips32_ir.h"
#include "mips/mips32_cpu.h"

#include "intel/i64_ir.h"
#include "intel/i64_cpu.h"

static const struct cpu_reg_map *_arch_to_map(
	_in enum mte_arch 	arch
){
	switch(arch){
	case MIPS32:
		return &_MIPS32_CPU_REG_MAP;
	case INTEL64:
		i64_load_qtables();
		return &_I64_CPU_REG_MAP;
	default:
		return nullptr;
	}
}
guest_to_ir_call arch_guest_to_ir(
	_in enum mte_arch 	arch
){
	switch(arch){
	case MIPS32:
		return (guest_to_ir_call)mips32_to_ir;
	case INTEL64:
		return (guest_to_ir_call)_invalid_guest_to_ir_call;
	default:
		return (guest_to_ir_call)_invalid_guest_to_ir_call;
	}
}
ir_to_host_call arch_ir_to_host(
	_in enum mte_arch 	arch
){
	switch(arch){
	case MIPS32:
		return (ir_to_host_call)_invalid_ir_to_host_call;
	case INTEL64:
		return (ir_to_host_call)i64_ir_to_raw;
	default:
		return (ir_to_host_call)_invalid_ir_to_host_call;
	}
}
guest_reg_fetch_call arch_guest_reg_fetch(
	_in enum mte_arch 	arch
){
	switch(arch){
	case MIPS32:
		return (guest_reg_fetch_call)mips32_reg_fetch;
	case INTEL64:
		return (guest_reg_fetch_call)_invalid_guest_reg_fetch_call;
	default:
		return (guest_reg_fetch_call)_invalid_guest_reg_fetch_call;
	}
}

#include <ax_utility.h>
_inline_avert axres ir_create(
	_in const u64 		version,
	_in const enum mte_arch guest_arch,
	_in const enum mte_arch host_arch,
	_in const u64 		code_size,
	_in const u64 		gen_size,
	_out ir_context		**buf
){
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	/*
	 	Controlled undefined behaviour (const overwrite)
	*/

	void *code_buf = axmalloc(code_size);
	void *gen_buf = axmalloc(gen_size);

	// Create temporary IR context
	ir_context temp_ir = (ir_context){
		.desc = {
			.guest = {
				.base = code_buf,
				.capacity = code_size,
			},
			.host = {
				.base = gen_buf,
				.capacity = gen_size,
			},

			.guest_ptr = code_buf,
			.host_ptr = gen_buf,

			.guest_map = _arch_to_map(guest_arch),
			.host_map = _arch_to_map(host_arch),

			.call = {
				.guest_to_ir = arch_guest_to_ir(guest_arch),
				.ir_to_host = arch_ir_to_host(host_arch),
				.guest_reg_fetch = arch_guest_reg_fetch(guest_arch),
			},

			.guest_arch = guest_arch,
			.host_arch = host_arch,
		},
		.blocked = false,
		.version = version,
	};
	ir_context *ir =
		axmalloc(sizeof(ir_context));
	memcpy(ir, &temp_ir, sizeof(ir_context));

	/*
	 	Prefetch and check if correct by calling each function
	*/
	if ((u64)ir->desc.call.guest_to_ir != (u64)_invalid_guest_to_ir_call){
		ir->desc.call.guest_to_ir(&(mte_raw_instr){0}, nullptr, nullptr);
	}
	if ((u64)ir->desc.call.ir_to_host != (u64)_invalid_ir_to_host_call){
		ir->desc.call.ir_to_host(&(ir_raw_instr){0}, nullptr, nullptr);
	}
	if ((u64)ir->desc.call.guest_reg_fetch != (u64)_invalid_guest_reg_fetch_call){
		ir->desc.call.guest_reg_fetch(&(mte_raw_instr){0}, nullptr, nullptr);
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

