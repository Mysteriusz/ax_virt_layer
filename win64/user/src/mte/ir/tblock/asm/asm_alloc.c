#include <ax_io.h>

#include "mte/cpu.h"
#include "mte/perf.h"

#include "mte/ir/tblock/asm/asm_alloc.h"

u16 asm_alloc_space(
	_in const struct cpu_reg_map 	*const reg_map,
	_in const enum cpu_reg_role 	role,
	_in_out asm_reg_state 		(*const state_map)[IR_REG_LIMIT]
){
	if (__builtin_expect(reg_map == nullptr, false)){
		return 0;
	}
	if (__builtin_expect(state_map == nullptr, false)){
		return 0;
	}

	u8 idx = 0;
	u8 count = 0;

	/*
	 	SUPER IMPORTANT TODO!!!!!!
		THE INDEX SHOULD NOT BE FIXED AS 0 SINCE IT REGISTER COUNT TO 64
	*/
	u64 mask = (*reg_map->role_map)[role][0];
	u32 n = __builtin_popcountll((*reg_map->role_map)[role][0]); // Count of all registers

	struct cpu_reg_desc *alloc = nullptr;
	while (count < n){
		idx = __builtin_ctz(mask) & 0x0f;
		count++;

		struct cpu_reg_desc *reg = &reg_map->root[idx];
		/*
		 	If register is not free then move
			mask left to increase the 'idx'

			Example:
				mask = 0x0C00

					0000110000000000

				When we shift by one left it becomes:

					00001000000000000

				Now ctz on the next iteration will return the next index to check
		*/
		if ((*state_map)[idx] != REG_FREE){
			mask &= ~BIT(idx);
			continue;
		}
		alloc = reg;
		break;
	}

	/*
	 	Spill present
	*/
	if (alloc == nullptr){
		return (1 << 8);
	}

	(*state_map)[idx] = REG_OCCUPIED;
	return alloc->id;
}

