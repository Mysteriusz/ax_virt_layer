#include <ax_io.h>

#include "mte/cpu.h"
#include "mte/perf.h"

u8 cpu_alloc_reg(
	_in struct cpu_reg_map 		*map,
	_in enum cpu_reg_role 		role
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
		if (reg->state != REG_FREE){
			mask >>= i + 1;
			continue;
		}
		alloc = reg;
		break;
	}

	// Manage spill
	if (alloc == nullptr){
		return 0;
	}

	alloc->state = REG_OCCUPIED;
	/*__INL_PERF_END
	__INL_PERF_LOG*/
	return alloc->id;
}

