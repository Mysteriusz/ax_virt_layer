#if !defined(MTE_TBLOCK_INT)
#define MTE_TBLOCK_INT

#include <ax_type.h>

#include "mte/ir/ir.h"

#define TBLOCK_SIZE 64

typedef struct _tblock_reg_assoc{
	bool 	used;
	u8	id;
} tblock_reg_assoc;
typedef u16 tblock_reg_liveness;

/*
 	A structure that holds code translation information
*/
typedef struct _tblock{
	enum tblock_type : u8{
		TBLOCK_BIG = 3, // Shift multiplier TBLOCK_SIZE*8
		TBLOCK_SMALL = 1, // Shift multiplier TBLOCK_SIZE*2
	} type;
	u16		ir_len; // Length of the 'ir_buf'
	ir_raw_instr	*ir_buf; // Heap buffer for translated instructions

	/*
	 	Each bit of the u16 represents the 16-byte aligned block,
		for each register identifier.
			
		IR instruction count for each block is:
			((TBLOCK_SIZE << block->type) / sizeof(ir_raw_instr))
	*/
	tblock_reg_liveness	liveness[0xff];
	/*
	 	Associations between org and tar registers.
	*/
	tblock_reg_assoc	assoc[0xff];
} tblock;

bool tblock_alloc(
	_in enum tblock_type 	type,
	_out tblock 		*buf
);

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

/*
 	Fills liveness state of each register,
	Sets the [block->ir_count] field.
*/
bool tblock_liveness_scan(
	_in ir_context 	*ir,
	_in_out tblock	*block
);

/*
 	Converts tar (guest) to IR instructions.
*/
bool tblock_raw_to_ir(
	_in ir_context	*ir,
	_in_out tblock	*block
);

/*
 	Converts IR to tar (host) instructions.
*/
bool tblock_ir_to_raw(
	_in_out ir_context	*ir,
	_in tblock		*block
);

/*
 	Exectues end-to-end translation process with emition.
*/
bool tblock_emit(
	_in_out ir_context	*ir,
	_in_out tblock 		*block
);

#endif // !defined(MTE_TBLOCK_INT)

