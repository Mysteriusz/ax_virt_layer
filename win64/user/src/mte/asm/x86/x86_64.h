#if !defined(MTE_X86_64_INT)
#define MTE_X86_64_INT

#include "mte/asm/decode_u64.h"
#include "mte/asm/x86/x86_64_instr.h"
#include "mte/cpu.h"
#include "mte/ir.h"

static const struct cpu_reg_map _x86_64_cpu_reg_map = {
	.gpr_count = 16, // 16 gprs registers rax-r15
	.reg_width = 64, // 64-Bit architecture
	.root = (reg64[16]){}
};

#endif

