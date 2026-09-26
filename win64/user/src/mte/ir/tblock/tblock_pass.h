#if !defined(MTE_TBLOCK_PASS_INT)
#define MTE_TBLOCK_PASS_INT

#include "mte/ir/tblock/tblock.h"

#define __TBLOCK_PASS_INIT(ir, block) \
	struct ir_context_desc *const _TBLOCK_DESC = &ir->desc; \
	u16 _TBLOCK_PASS_IDX = 0; \
	tblock *const _TBLOCK = block; \
	u8 *_TBLOCK_GUEST_PTR = _TBLOCK_DESC->guest_ptr; \
	u8 *_TBLOCK_GUEST_PTR_CEIL = offp(_TBLOCK_GUEST_PTR, TBLOCK_SIZE << block->type); \
	const u32 _TBLOCK_FRAG = (TBLOCK_SIZE << block->type) / sizeof(ir_raw_instr);

#define __TBLOCK_PASS_LOOP(pass_len, ...) ({ \
	/* Check if code_ptr exceeds the tblock bounds */ \
	while(_TBLOCK_GUEST_PTR < _TBLOCK_GUEST_PTR_CEIL){ \
		__VA_ARGS__ \
		/* Calculate new offset */ \
		_TBLOCK_GUEST_PTR = offp(_TBLOCK_GUEST_PTR, pass_len); \
		_TBLOCK_PASS_IDX++; \
		asrt(pass_len > 0); \
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
	_in ir_context 	ir _prepass_with(const restrict),
	_in_out tblock	block _prepass_with(const restrict)
);

/*
 	Converts host (guest) to IR instructions.
*/
struct tblock_pass_result tblock_raw_to_ir(
	_in ir_context	ir _prepass_with(const restrict),
	_in_out tblock	block _prepass_with(const restrict)
);

/*
 	Converts IR to host (host) instructions.
*/
struct tblock_pass_result  tblock_ir_to_raw(
	_in_out ir_context	ir _prepass_with(const restrict),
	_in tblock		block _prepass_with(const restrict)
);

#endif // !defined(MTE_TBLOCK_PASS_INT)

