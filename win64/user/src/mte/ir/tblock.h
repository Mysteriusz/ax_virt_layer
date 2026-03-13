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
	u32		*base;
	ir_context	*ir;
} tblock;

bool tblock_alloc(
	_in ir_context		*ir,
	_in enum tblock_type 	type,
	_out tblock		*buf
);

#define __TBLOCK_PASS_INIT(block) \
	tblock *pass_blk = block; \
	u32 *code_p0 = block->base; \
	u32 *code_p1 = (u32*)offp(block->base, 4); \
	u32 *code_p2 = (u32*)offp(block->base, 8); \
	u32 *code_p3 = (u32*)offp(block->base, 12);

#define __TBLOCK_PASS_LOOP(pass_len, ...) ({ \
	while(code_p0 < (u32*)offp(pass_blk->base, TBLOCK_SIZE << pass_blk->type)){ \
		__VA_ARGS__ \
		/* \
		 	Calculate new offsets \
		*/ \
		++code_p0; \
		code_p1++; \
		code_p2++; \
		code_p3++; \
	} \
})
	
bool tblock_liveness_scan(
	_in tblock	*block,
	_in_out	u16	liveness[0xff]
);
bool tblock_raw_to_ir(
	_in tblock	*block
);
bool tblock_emit(
	_in tblock 	*block
);

