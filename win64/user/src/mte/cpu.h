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
	REG_PRESERVE 		= 0x03, // Ex: i64.r11

	REG_STACK		= 0x08, // Ex: mips32.a0

	REG_STACK_PTR		= 0x10, // Ex: i64.rsi
	REG_FRAME_PTR		= 0x11, // Ex: mips32.fp
	REG_MEM_PTR		= 0x12, // Ex: mips32.gp
};
struct cpu_reg_desc{
	reg64			value; // Register placeholder
	enum cpu_reg_role 	role;
	u8			id; // Architecture specific register identifier
};

struct cpu_reg_map{
	const u16		reg_count; // Register count
	const u16		reg_width; // Max register width
	u16			(*role_map)[0xff]; // Register id to role map
	struct cpu_reg_desc	*const root; // Register array
};

typedef struct _cpu_state{
	struct cpu_reg_map 	gprs;
} cpu_state;

#endif // !defined(MTE_CPU_INT)

