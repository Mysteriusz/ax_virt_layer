#include "mte/core.h"
#include "reg.h"

enum cpu_reg{
	R0, // Zero
	R1, // Reserved for asm
	R2, R3, // Results
	R4, R5, R6, R7, // Arguments
	R8, R9, R10, R11, R12, R13, R14, R15, // Temporaries
	R16, R17, R18, R19, R20, R21, R22, R23, // Saved
	R24, R25, // Not saved
	R26, R27, // OS reserved
	R28, // Global pointer
	R29, // Stack pointer
	R30, // Frame pointer
	R31 // Return address
};

#define GPR_COUNT 32 // General purpose registers
struct cpu{
	reg gpr[GPR_COUNT];
};

axres cpu_init(
	_in_out struct cpu 	*cpu
);

