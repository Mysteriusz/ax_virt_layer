#include <ax_type.h>

#include "mte/cpu.h"

#include "ir.h"

#define TBLOCK_SIZE 128

typedef struct _tblock{
	enum tblock_type : u8{
		TBLOCK_BIG = 3, // Shift multiplier TBLOCK_SIZE*8
		TBLOCK_SMALL = 1, // Shift multiplier TBLOCK_SIZE*2
	} type;
	u32		*start;
	u32		*end;
	ir_context	*ir;
} tblock;

bool tblock_alloc(
	_in ir_context		*ir,
	_in enum tblock_type 	type,
	_out tblock		*buf
);

bool tblock_emit(
	_in tblock 	*block
);

