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

/*

 	Ir context call interface

*/

// Translate org (guest) asm to IR
typedef ir_raw_instr (*const org_to_ir_call)(
	_in mte_raw_instr 	instr,
	_in ir_context 		*ctx,
	_out u8			*len // Original instruction length (in bytes)
);
static ir_raw_instr _invalid_org_to_ir_call(
	...
){
	io_str(u"Invalid guest to ir call");
	exit(1);
}

// Translate IR to tar (host) asm
typedef mte_raw_instr (*const ir_to_tar_call)(
	_in ir_raw_instr 	instr,
	_in ir_context 		*ctx,
	_out u8			*len // Target instruction length (in bytes)
);
static mte_raw_instr _invalid_ir_to_tar_call(
	...
){
	io_str(u"Invalid ir to host call");
	exit(1);
}

// Fetch all register id`s from guest instruction 
typedef ir_operand_set (*const org_reg_fetch_call)(
	_in mte_raw_instr 	instr,
	_in ir_context 		*ctx,
	_out u8			*len
);
static ir_operand_set _invalid_org_reg_fetch_call(
	...
){
	io_str(u"Invalid guest register fetch call");
	exit(1);
}

#define IR_SPILL_LIMIT 0xff
struct ir_sbuf{ _align(64)
	u8	idx;
	u64	base[IR_SPILL_LIMIT];
};

static u8 _ir_sbuf_inc(struct ir_sbuf *sbuf){
	if (__builtin_expect(sbuf == nullptr, false)){
		return 0xff;
	}

	sbuf->idx++;
	if (__builtin_expect(sbuf->idx == 0xff, false)){
		ax_log_msg(AX_UNK_ERR, u"Static buffer out of bounds!!");
		asrt(false);
	}
	return sbuf->idx - 1;
}

struct ir_context_desc{
	// Cache line
	enum mte_arch 			org_arch;
	enum mte_arch 			tar_arch;
	struct cpu_reg_map *const 	org_map;
	struct cpu_reg_map *const	tar_map;

	// Cache line
	const struct{
		/*
		 	TODO:
				tar_reg_fetch_call 	; Translate registers from tar (host) to IR representation
				
				QUESTIONABLE:
				org_len_resolve_call	; Resolve length of org (guest) raw instruction
				tar_len_resolve_call 	; Resolve length of tar (host) raw instruction 
		*/
		org_to_ir_call 		org_to_ir;
		ir_to_tar_call 		ir_to_tar;
		org_reg_fetch_call 	org_reg_fetch;
	} call;
	// Cache line
	u8 *const 			code_base;
	u8 *const 			gen_base;
	u8				*code_ptr;
	u8				*gen_ptr;

	// Multiple cache lines
	/*struct ir_sbuf 			imm_buf;
	struct ir_sbuf 			ptr_buf;*/
};

typedef struct _ir_context{
	const u64 		version; // Ex: 0.01\0, 123.45\0
	_Atomic bool 		blocked;
	struct ir_context_desc 	desc;
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

org_to_ir_call arch_org_to_ir(
	_in enum mte_arch 	arch
);
ir_to_tar_call arch_ir_to_tar(
	_in enum mte_arch 	arch
);

#endif // !defined(MTE_IR_CORE_INT)

