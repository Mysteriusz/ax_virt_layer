#if !defined(MTE_CPU_INT)
#define MTE_CPU_INT

#include <ax_type.h>

#include "mte/arch.h"

typedef u8 	reg8;
typedef u16 	reg16;
typedef u32 	reg32;
typedef u64 	reg64;

struct cpu_reg_map{
	u16	gpr_count; // General purpose register count
	u8	reg_width; // Max register width
	void	*root; // Register array
};

const static struct cpu_reg_map empty = (struct cpu_reg_map){0};
_unused
const static struct cpu_reg_map _mips32_cpu_reg_map = {
	.gpr_count = 32, // 32 registers r0-r31
	.reg_width = 32, // 32-Bit architecture
	.root = (reg64[32]){}
};
_unused
const static struct cpu_reg_map _i64_cpu_reg_map = {
	.gpr_count = 16, // 16 gprs registers rax-r15
	.reg_width = 64, // 64-Bit architecture
	.root = (reg64[16]){}
};

static const struct cpu_reg_map *const _arch_to_map(
	_in enum mte_arch 	arch
){
	switch(arch){
	case MIPS32:
		return &_mips32_cpu_reg_map;
	case INTEL64:
		return &_i64_cpu_reg_map;
	default:
		return &empty;
	}
}

typedef struct _cpu_state{
	struct cpu_reg_map gprs;
} cpu_state;

#endif // !defined(MTE_CPU_INT)

