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
enum cpu_reg_state : u8{
	REG_FREE = 0,
	REG_OCCUPIED = 1,
};
struct cpu_reg_desc{
	u64			value; // Register placeholder
	enum cpu_reg_role 	role;
	enum cpu_reg_state	state;
	u8			id; // Architecture specific register identifier
};

typedef u64 cpu_spill_buffer[32];
struct cpu_reg_map{
	u16			reg_count; // Register count
	u8			reg_width; // Max register width
	/*
	 	Spill buffer allocation map
	*/
	u32			spill_map;
	cpu_spill_buffer 	spill; // Used for storage when out of registers
	struct cpu_reg_desc	*root; // Register array
};

typedef struct _cpu_state{
	struct cpu_reg_map 	gprs;
} cpu_state;

extern struct cpu_reg_map _MIPS32_CPU_REG_MAP;
extern struct cpu_reg_map _I64_CPU_REG_MAP;

/*
 	Automatically allocate register with spill management
*/
u8 cpu_alloc_reg(
	_in struct cpu_reg_map 		*map,
	_in enum cpu_reg_role 		role
);


#endif // !defined(MTE_CPU_INT)

