#if !defined(MTE_I64_CPU_INT)
#define MTE_I64_CPU_INT 

#include "mte/cpu.h"

#include "intel/instr/i64_instr.h"
#include "intel/instr/i64_operand.h"

#define I64_REG_COUNT 16
#define I64_REG_WIDTH 64

extern u16 _I64_CPU_REG_ROLE_MAP[0xff];
extern struct cpu_reg_map _I64_CPU_REG_MAP;

_inline_avert void i64_load_cpu_reg_map(
	void
);

#endif // !defined(MTE_I64_CPU_INT)

