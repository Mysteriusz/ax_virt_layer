#include "cpu.h"

axres cpu_init(
	_in_out struct cpu 	*cpu
){
	if (cpu == nullptr){
		return AX_INV_ARG;
	}

	memset(cpu->gpr, 0, sizeof(cpu->gpr));
	return AX_SUCC;
}

