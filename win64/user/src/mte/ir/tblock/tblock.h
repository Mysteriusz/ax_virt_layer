#if !defined(MTE_TBLOCK_INT)
#define MTE_TBLOCK_INT

#include <ax_type.h>

#include "mte/ir/ir.h"

#include "asm/asm_types.h"

#define TBLOCK_SIZE 64 // Byte size of the default tblock size

/*
 	A structure that holds code translation information
*/
typedef struct _align(64) _tblock{
	const enum tblock_type : u8{
		TBLOCK_BIG = 3, // Shift multiplier TBLOCK_SIZE*8
		TBLOCK_SMALL = 1, // Shift multiplier TBLOCK_SIZE*2
	} type;

	u16				ir_cnt; // Count of instructions written to 'ir_buf.ptr'
	__IR_DATA_BUFFER(ir_raw_instr) 	ir_buf;

	/*
	 	Each bit of the u16 represents the 16-byte aligned block,
		for each register identifier.
			
		IR instruction count for each block is:
			((TBLOCK_SIZE << block->type) / sizeof(ir_raw_instr))
	*/
	asm_reg_liveness	liveness[IR_REG_LIMIT];
	/*
	 	Associations between guest (guest) and host (host) registers.
	*/
	asm_reg_assoc		assoc[IR_SPILL_REG_LIMIT];
	/*
	 	States of individual guest (guest) registers
	*/
	asm_reg_state		state[IR_SPILL_REG_LIMIT];
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
inline u32 _tblock_frag_calc(
	_in enum tblock_type type
){
	return ((TBLOCK_SIZE << type) / sizeof(ir_raw_instr));
}

#define TBLOCK_IR_BUFFER_SIZE KIB(4)
static_asrt(divide_compatible(TBLOCK_IR_BUFFER_SIZE, sizeof(ir_raw_instr)));

tblock* tblock_alloc(
	_in const enum tblock_type 	type
);

/*
 	Exectues end-to-end translation process with emition
	to ir code pointer
*/
bool tblock_emit(
	_in_out ir_context	*ir,
	_in_out tblock 		*const block
);

#endif // !defined(MTE_TBLOCK_INT)

