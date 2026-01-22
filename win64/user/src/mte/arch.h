#include <ax_type.h>

enum mte_arch : u32{
	MTE_ARCH_MIN,
	INTEL64 = 646 ^ (1UL << 31), // ("intel64" ascii sum) ^ (0x80000000)
	MIPS32 = 542 ^ (1UL << 31), // ("mips32" ascii sum) ^ (0x80000000)
	MTE_ARCH_MAX
};
enum mte_syn : u32{
	MTE_SYN_MIN,
	INTEL = 1,
	ATNT = 2,
	MTE_SYN_MAX
};

