#include <ax_io.h>

#include "mte/cpu.h"
#include "mte/perf.h"

u8 cpu_alloc_role_reg(
	_in struct cpu_reg_map 	*map,
	_in enum cpu_reg_role 	role
){
	/*__INL_PERF_INIT
	__INL_PERF_START*/
	if (__builtin_expect(map == nullptr, false)){
		return 0;
	}

	u16 mask = (*map->role_map)[role];
	u32 n = __builtin_popcount(mask);
	u8 i = 0;
	u8 cnt = 0;

	struct cpu_reg_desc *alloc = nullptr;
	while (cnt < n){
		i = __builtin_ctz(mask);
		cnt++;

		struct cpu_reg_desc *reg = &map->root[i];
		/*
		 	If register is not free then move
			mask left to increase the 'i'

			Example:
				mask = 0x0C00

					0000110000000000

				When we shift by one left it becomes:

					0001100000000000

				Now ctz will return one one index more
		*/
		if (reg->state != REG_FREE){
			mask <<= 1;
			continue;
		}
		alloc = reg;
		break;
	}

	/*
	 	TODO:
		Manage spill

		This can either mean changing the role to search for 
		or just using the map`s heap buffer that holds spill
	*/
	if (alloc == nullptr){
		return 0;
	}

	alloc->state = REG_OCCUPIED;
	/*__INL_PERF_END
	__INL_PERF_LOG*/
	return alloc->id;
}

void cpu_free_reg(
	_in struct cpu_reg_map 	*map,
	_in u8 			reg
){
	if (__builtin_expect(map == nullptr, false)){
		return;
	}
	map->root[reg].state = REG_FREE;
}

