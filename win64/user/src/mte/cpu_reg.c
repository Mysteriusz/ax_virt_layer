#include "mte/cpu.h"
#include "mte/perf.h"

u8 cpu_alloc_reg(
	_in struct cpu_reg_map 		*map,
	_in enum cpu_reg_role 		role
){
	__INL_PERF_INIT
	__INL_PERF_START
	if (__builtin_expect(map == nullptr, false)){
		return 0;
	}

	struct cpu_reg_desc *alloc = nullptr;
	for (u8 i = 0; i < map->reg_count; i++){
		struct cpu_reg_desc *reg = &map->root[i];
		if (reg->state != REG_FREE){
			continue;
		}
		if (reg->role != role){
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
	__INL_PERF_END
	__INL_PERF_LOG
	return alloc->id;
}

