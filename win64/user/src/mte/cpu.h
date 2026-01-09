#include <ax_type.h>

typedef u8 	reg8;
typedef u16 	reg16;
typedef u32 	reg32;
typedef u64 	reg64;

struct cpu_reg_map{
	u16	gpr_count; // General purpose register count
	u8	reg_width; // Max register width
	void	*root; // Register array
};

typedef struct _cpu_state{
} cpu_state;

