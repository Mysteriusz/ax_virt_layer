#include <ax_io.h>

#include "mte/cpu.h"
#include "mte/perf.h"

#include "asm_alloc.h"

u8 asm_alloc_spill(
	_in_out asm_reg_state 		(*state_map)[IR_SPILL_REG_LIMIT]
){
	if (__builtin_expect(state_map == nullptr, false)){
		return 0;
	}

	u16 i = 0;
	while(i < IR_SPILL_LIMIT){
		if ((*state_map)[IR_REG_LIMIT + i] == REG_FREE){
			(*state_map)[IR_REG_LIMIT + i] = REG_OCCUPIED;
			return i;
		}
		i++;
	}
	return 0;
}

u16 asm_alloc_reg(
	_in const struct cpu_reg_map 	*reg_map,
	_in enum cpu_reg_role 		role,
	_in_out asm_reg_state 		(*state_map)[IR_SPILL_REG_LIMIT]
){
	if (__builtin_expect(reg_map == nullptr, false)){
		return 0;
	}
	if (__builtin_expect(state_map == nullptr, false)){
		return 0;
	}

	u8 idx = 0;
	u8 count = 0;

	u16 mask = (*reg_map->role_map)[role];
	u32 n = __builtin_popcount((*reg_map->role_map)[role]); // Count of all registers

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
	 	Allocate spill index
	*/
	if (alloc == nullptr){
		return asm_alloc_spill(state_map) << 8 | 0xff;
	}

	(*state_map)[idx] = REG_OCCUPIED;
	return 0xff << 8 | alloc->id;
}

