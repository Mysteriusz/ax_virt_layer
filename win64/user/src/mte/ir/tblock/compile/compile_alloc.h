#if !defined(MTE_COMPILE_ALLOC_INT)
#define MTE_COMPILE_ALLOC_INT

#include "mte/cpu.h"
#include "mte/ir/ir.h"

#include "compile_types.h"


/*
 	Allocate spill space and calculate the offset of the 
*/
u8 comp_alloc_spill(
	_in_out comp_reg_state 	(*state_map)[0xff + IR_SPILL_LIMIT]
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
u16 comp_alloc_reg(
	_in const struct cpu_reg_map 	*reg_map,
	_in enum cpu_reg_role 		role,
	_in_out comp_reg_state 		(*state_map)[0xff + IR_SPILL_LIMIT]
);

#endif // !defined(MTE_TBLOCK_CPU_INT)

