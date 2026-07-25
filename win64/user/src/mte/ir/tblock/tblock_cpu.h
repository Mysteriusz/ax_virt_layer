#if !defined(MTE_TBLOCK_CPU_INT)
#define MTE_TBLOCK_CPU_INT

#include "mte/cpu.h"

enum tblock_reg_state : u8{
	REG_FREE = 0,
	REG_OCCUPIED = 1,
};

/*
 	Allocate spill space and calculate the offset of the 
*/
u8 tblock_alloc_spill(
	_in enum tblock_reg_state 	(*state_map)[0xff + CPU_SPILL_LIMIT]
);

/*
 	Automatically allocate any register with spill management
	for a specific role

	Return u16 is defined as following:

	(SSSSSSSSRRRRRRRR)

	S -> Spill 8 bit index
	R -> Register 8 bit index

	Since each field is an index then
	field of value 0xff is a special case
		
	If spill is present then 
	the 8 LSB bits (R) are 0xff

	Example:
		spill_idx << 8 | 0xff

	If spill is not present then 
	the 8 MSB bits (S) are 0xff

	Example:
		0xff | reg_idx
*/
u16 tblock_alloc_reg(
	_in enum tblock_reg_state 	(*state_map)[0xff + CPU_SPILL_LIMIT],
	_in struct cpu_reg_map 		*reg_map,
	_in enum cpu_reg_role 		role
);

void tblock_free_reg(
	_in enum tblock_reg_state 	(*state_map)[0xff + CPU_SPILL_LIMIT],
	_in u8 				reg
);

#endif // !defined(MTE_TBLOCK_CPU_INT)

