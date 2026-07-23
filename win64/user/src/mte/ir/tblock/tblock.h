#if !defined(MTE_TBLOCK_INT)
#define MTE_TBLOCK_INT

#include <ax_type.h>

#include "mte/ir/ir.h"

#define TBLOCK_SIZE 64

/*
 	A structure that holds code translation information
*/
typedef struct _tblock{
	enum tblock_type : u8{
		TBLOCK_BIG = 3, // Shift multiplier TBLOCK_SIZE*8
		TBLOCK_SMALL = 1, // Shift multiplier TBLOCK_SIZE*2
	} type;
	u16		ir_count;
	u8	*base; // Acts as ir->desc.code_ptr (should be removed)
	ir_context	*ir;
	ir_raw_instr	*ir_buf; // Heap buffer for translated instructions
} tblock;

bool tblock_alloc(
	_in ir_context		*ir,
	_in enum tblock_type 	type,
	_out tblock		*buf
);

#define __TBLOCK_PASS_INIT(block) \
	struct ir_context_desc *const desc = &block->ir->desc; \
	u16 pass_i = 0; \
	tblock *pass_block = block; \
	u8 *code_ptr = block->base;

#define __TBLOCK_PASS_LOOP(pass_len, ...) ({ \
	/* Check if code_ptr exceeds the tblock bounds */ \
	while(code_ptr < offp(pass_block->base, TBLOCK_SIZE << pass_block->type)){ \
		__VA_ARGS__ \
		/* Calculate new offset */ \
		code_ptr += pass_len; \
		pass_i++; \
	} \
})

typedef struct _tblock_reg_assoc{
	bool 	used;
	u8	id;
} tblock_reg_assoc;

/*
 	Fills liveness state of each register,
	Sets the [block->ir_count] field.
*/
bool tblock_liveness_scan(
	_in_out tblock	*block,
	_in_out	u16	org_liveness[0xff]
);

/*
 	Converts tar (guest) to IR instructions.
*/
bool tblock_raw_to_ir(
	_in tblock			*block,
	_in const u16			org_liveness[0xff], // Liveness table of guest registers (Per instruction block)
	_in_out tblock_reg_assoc 	assoc[0xff] // Guest to host register associations (Per instruction block)
);

/*
 	Converts IR to tar (host) instructions.
*/
bool tblock_ir_to_raw(
	_in tblock	*block
);

/*
 	Exectues end-to-end translation process with emition.
*/
bool tblock_emit(
	_in tblock 	*block
);

#endif // !defined(MTE_TBLOCK_INT)

