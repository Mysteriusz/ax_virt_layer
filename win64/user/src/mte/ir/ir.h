#if !defined(MTE_IR_CORE_INT)
#define MTE_IR_CORE_INT

#include <ax_error.h>
#include <ax_memory.h>
#include <stdatomic.h>

#include "mte/arch.h"
#include "mte/cpu.h"
#include "mte/core.h"

#include "mte/ir/ir_instr.h"

#define IR_LATEST		*(u64*)"0.01"

typedef struct _ir_context ir_context;

#define __IR_DATA_BUFFER(type) struct{ \
	const u32 capacity; \
	type *const base; \
}

/*

 	Ir context call interface

*/

// Translate guest (guest) asm to IR
typedef ir_raw_instr (*const guest_to_ir_call)(
	_in mte_raw_instr 	*instr,
	_in ir_context 		*ctx,
	_out u8			*len // Original instruction length (in bytes)
);
static ir_raw_instr _invalid_guest_to_ir_call(
	...
){
	io_str(u"Invalid guest to ir call");
	exit(1);
}

// Translate IR to host (host) asm
typedef mte_raw_instr (*const ir_to_host_call)(
	_in ir_raw_instr 	*instr,
	_in ir_context 		*ctx,
	_out u8			*len // Target instruction length (in bytes)
);
static mte_raw_instr _invalid_ir_to_host_call(
	...
){
	io_str(u"Invalid ir to host call");
	exit(1);
}

// Fetch all register id`s from (guest) instruction 
typedef ir_operand_set (*const guest_reg_fetch_call)(
	_in mte_raw_instr 	*instr,
	_in ir_context 		*ctx,
	_out u8			*len
);
static ir_operand_set _invalid_guest_reg_fetch_call(
	...
){
	io_str(u"Invalid guest register fetch call");
	exit(1);
}

#define IR_REG_LIMIT 0x100
struct ir_context_desc{
	// Cache line

	__IR_DATA_BUFFER(u8) 	guest;
	__IR_DATA_BUFFER(u8) 	host;

	u8 			*guest_ptr;
	u8 			*host_ptr;

	const struct cpu_reg_map 	*const guest_map;
	const struct cpu_reg_map 	*const host_map;
	
	// Cache line

	const struct _align(64){
		/*
		 	TODO:
				host_reg_fetch_call 	; Translate registers from host (host) to IR representation
		*/
		guest_to_ir_call 	guest_to_ir;
		ir_to_host_call 	ir_to_host;
		guest_reg_fetch_call 	guest_reg_fetch;
	} call;

	// Cache line

	const enum mte_arch 		guest_arch;
	const enum mte_arch 		host_arch;
};
static_asrt(divide_compatible(sizeof(struct ir_context_desc), 64));

typedef struct _ir_context{
	struct ir_context_desc 	desc;
	const u64 		version; // IR_BASE_VERSION
	_Atomic bool 		blocked;
} ir_context;

_inline_avert axres ir_create(
	_in const u64 		version,
	_in const enum mte_arch guest_arch,
	_in const enum mte_arch host_arch,
	_in const u64 		code_size,
	_in const u64 		gen_size,
	_out ir_context		**buf
);

_inline_avert void ir_delete(
	_in ir_context 		*ir
);

guest_to_ir_call arch_guest_to_ir(
	_in enum mte_arch 	arch
);
ir_to_host_call arch_ir_to_host(
	_in enum mte_arch 	arch
);

#endif // !defined(MTE_IR_CORE_INT)

