#if !defined(MTE_TBLOCK_PASS_INT)
#define MTE_TBLOCK_PASS_INT

#include "tblock.h"

#define __TBLOCK_PASS_INIT(ir, block) \
	struct ir_context_desc *const _TBLOCK_DESC = &ir->desc; \
	u16 _TBLOCK_PASS_IDX = 0; \
	tblock *const _TBLOCK_PASS_BLOCK = block; \
	u8 *_TBLOCK_CODE_PTR = _TBLOCK_DESC->code_base; \
	u8 *const _TBLOCK_CODE_PTR_CEIL = offp(_TBLOCK_DESC->code_base, TBLOCK_SIZE << block->type); \
	const u32 _TBLOCK_FRAG = (TBLOCK_SIZE << block->type) / sizeof(ir_raw_instr);

#define __TBLOCK_PASS_LOOP(pass_len, ...) ({ \
	/* Check if code_ptr exceeds the tblock bounds */ \
	while(_TBLOCK_CODE_PTR < _TBLOCK_CODE_PTR_CEIL){ \
		__VA_ARGS__ \
		/* Calculate new offset */ \
		_TBLOCK_CODE_PTR = offp(_TBLOCK_CODE_PTR, pass_len); \
		_TBLOCK_PASS_IDX++; \
	} \
})

struct tblock_pass_result{
	u32 	count;
	axres 	res;
};


/*
 	Fills liveness state of each register,
	Sets the [block->ir_count] field.
*/
struct tblock_pass_result tblock_liveness_scan(
	_in ir_context 	*ir,
	_in_out tblock	*block
);

/*
 	Converts tar (guest) to IR instructions.
*/
struct tblock_pass_result tblock_raw_to_ir(
	_in ir_context	*ir,
	_in_out tblock	*block
);

/*
 	Converts IR to tar (host) instructions.
*/
struct tblock_pass_result  tblock_ir_to_raw(
	_in_out ir_context	*ir,
	_in tblock		*block
);

#endif // !defined(MTE_TBLOCK_PASS_INT)

