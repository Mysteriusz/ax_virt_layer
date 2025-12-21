#include <ax_type.h>

typedef u8 	reg8;
typedef u16 	reg16;
typedef u32 	reg32;
typedef u64 	reg64;

struct cpu_reg_map{
	u16	reg_count;
	u8	reg_width;
	void	*root;
};

typedef struct _cpu_state{
} cpu_state;

