#if !defined(MTE_CPU_INT)
#define MTE_CPU_INT

#include <ax_type.h>

#include "mte/arch.h"

typedef u8 	reg8;
typedef u16 	reg16;
typedef u32 	reg32;
typedef u64 	reg64;

struct cpu_reg_id{
	u16		width;
	u8		id; // Architecture specific register identifier
	void		*mem; // Memory location of the register
};
struct cpu_reg_map{
	u16			reg_count; // Register count
	u8			reg_width; // Max register width
	struct cpu_reg_id	*root; // Register array
};

const static struct cpu_reg_map empty = (struct cpu_reg_map){0};
_unused
const static struct cpu_reg_map _mips32_cpu_reg_map = {
	.reg_count = 32, // 32 registers r0-r31
	.reg_width = 32, // 32-Bit architecture
	//.root = (reg64[32]){}
};
_unused
const static struct cpu_reg_map _i64_cpu_reg_map = {
	.reg_count = 16, // 16 gprs registers rax-r15
	.reg_width = 64, // 64-Bit architecture
	//.root = (reg64[16]){}
};

typedef struct _cpu_state{
	struct cpu_reg_map gprs;
} cpu_state;

#endif // !defined(MTE_CPU_INT)

