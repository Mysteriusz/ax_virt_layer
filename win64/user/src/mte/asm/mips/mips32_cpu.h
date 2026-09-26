#if !defined(MTE_MIPS32_CPU_INT)
#define MTE_MIPS32_CPU_INT 

#include "mte/cpu.h"

#include "mips/mips32_instr.h"

#define MIPS32_REG_COUNT 32
#define MIPS32_REG_WIDTH 32

extern const u64 _MIPS32_CPU_REG_ROLE_MAP[REG_ROLE_MAX][4];
extern const struct cpu_reg_map _MIPS32_CPU_REG_MAP;

#endif // !defined(MTE_MIPS32_CPU_INT)

