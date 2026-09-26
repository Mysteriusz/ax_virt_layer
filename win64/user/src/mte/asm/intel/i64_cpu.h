#if !defined(MTE_I64_CPU_INT)
#define MTE_I64_CPU_INT 

#include "mte/cpu.h"

#include "intel/instr/i64_instr.h"
#include "intel/instr/i64_operand.h"

#define I64_REG_COUNT 16
#define I64_REG_WIDTH 64

extern const u64 _I64_CPU_REG_ROLE_MAP[REG_ROLE_MAX][4];
extern const struct cpu_reg_map _I64_CPU_REG_MAP;

#endif // !defined(MTE_I64_CPU_INT)

