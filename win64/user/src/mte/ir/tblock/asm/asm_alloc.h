#if !defined(MTE_COMPILE_ALLOC_INT)
#define MTE_COMPILE_ALLOC_INT

#include "mte/cpu.h"
#include "mte/ir/ir.h"

#include "mte/ir/tblock/asm/asm_types.h"

/*
 	Allocate role-specific register or spill

	Return u16 is defined as following:

	(000000I RRRRRRRR)

	I -> Is spilled
	R -> Register 8 bit index

	If spill is present then 
	the S bit is set and 'state_map' remains untouched

	It is up to the caller to decide
	what spill being present does
*/
u16 asm_alloc_space(
	_in const struct cpu_reg_map 	*const reg_map,
	_in const enum cpu_reg_role 	role,
	_in_out asm_reg_state 		(*const state_map)[IR_REG_LIMIT]
);

#endif // !defined(MTE_TBLOCK_CPU_INT)

