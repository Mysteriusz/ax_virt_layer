#include <ax_type.h>

#include "mte/cpu.h"
#include "mte/perf.h"

#include "ir.h"

#define TBLOCK_SIZE 64

typedef struct _tblock{
	enum tblock_type : u8{
		TBLOCK_BIG = 3, // Shift multiplier TBLOCK_SIZE*8
		TBLOCK_SMALL = 1, // Shift multiplier TBLOCK_SIZE*2
	} type;
	u16		ir_count;
	u8		*base;
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
	u8 *code_p0 = block->base; \
	u8 *code_p1 = block->base + 4; \
	u8 *code_p2 = block->base + 8; \
	u8 *code_p3 = block->base + 12;

#define __TBLOCK_PASS_LOOP(pass_len, ...) ({ \
	while(code_p0 < offp(pass_block->base, TBLOCK_SIZE << pass_block->type)){ \
		__VA_ARGS__ \
		/* \
		 	Calculate new offsets \
		*/ \
		code_p0 += 4; \
		code_p1 += 8; \
		code_p2 += 12; \
		code_p3 += 16; \
		pass_i++; \
	} \
})


typedef struct _tblock_reg_assoc{
	bool 	used;
	u8	id;
} tblock_reg_assoc;

bool tblock_liveness_scan(
	_in tblock	*block,
	_in_out	u16	org_liveness[0xff]
);
bool tblock_raw_to_ir(
	_in tblock		*block,
	_in const u16		org_liveness[0xff], // Liveness table of guest registers (Per instruction block)
	_in tblock_reg_assoc	assoc[0xff] // Guest to host register associations (Per instruction block)
);
bool tblock_ir_to_raw(
	_in tblock	*block
);
bool tblock_emit(
	_in tblock 	*block
);

