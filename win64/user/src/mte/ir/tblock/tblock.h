#if !defined(MTE_TBLOCK_INT)
#define MTE_TBLOCK_INT

#include <ax_type.h>

#include "mte/ir/ir.h"

#include "tblock_cpu.h"

#define TBLOCK_SIZE 64

typedef struct _tblock_reg_assoc{
	bool 	used;
	bool 	spill;
	u8	id; // Host architecture-specific register id
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
	tblock_reg_assoc	assoc[0xff + CPU_SPILL_LIMIT];
	/*
	 	States of individual registers
	*/
	enum tblock_reg_state	state[0xff + CPU_SPILL_LIMIT];
} tblock;

/*
	Fragmentation of each block

	That means that every tblock is
	fragmented into frag-count 'subblocks'

	Example for TBLOCK_SMALL:

		(TBLOCK_SIZE << TBLOCK_SMALL) / sizeof(ir_raw_instr)
		which resolves to:
		(64 << 1) / 16 = 128 / 16 = 8

	Visualisation:
		block{
			frag {16 bytes} -> 8 times
		}
		
	Example for TBLOCK_BIG:

		(TBLOCK_SIZE << TBLOCK_BIG) / sizeof(ir_raw_instr)
		which resolves to:
		(64 << 3) / 16 = 512 / 16 = 16

	Visualisation:
		block{
			frag {16 bytes} -> 16 times
		}
*/
_inline_force u32 tblock_frag_calc(
	_in enum tblock_type type
){
	return ((TBLOCK_SIZE << type) / sizeof(ir_raw_instr));
}

bool tblock_alloc(
	_in enum tblock_type 	type,
	_out tblock 		*buf
);

/*
 	Exectues end-to-end translation process with emition
	to ir code pointer
*/
bool tblock_emit(
	_in_out ir_context	*ir,
	_in_out tblock 		*block
);

#endif // !defined(MTE_TBLOCK_INT)

