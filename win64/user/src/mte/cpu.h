#if !defined(MTE_CPU_INT)
#define MTE_CPU_INT

#include <ax_type.h>

#include "mte/arch.h"

typedef u8 	reg8;
typedef u16 	reg16;
typedef u32 	reg32;
typedef u64 	reg64;

enum cpu_reg_role : u8{
	REG_RETURN 		= 0x01, // Ex: i64.rax
	REG_TEMP 		= 0x02, // Ex: i64.rcx
	REG_PRESERVE 		= REG_TEMP | 0x04, // Ex: i64.r11
	REG_STACK		= 0x08, // Ex: mips32.a0
	REG_STACK_PTR		= 0x10, // Ex: i64.rsi
	REG_MEM_PTR		= 0x20, // Ex: i64.rsi
};
struct cpu_reg_desc{
	u16			width;
	enum cpu_reg_role 	role;
	u8			id; // Architecture specific register identifier
	void			*mem; // Memory location of the register
};

struct cpu_reg_map{
	u16			reg_count; // Register count
	u8			reg_width; // Max register width
	struct cpu_reg_desc	*root; // Register array
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

