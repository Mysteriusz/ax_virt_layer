#if !defined(MTE_ARCH_INT)
#define MTE_ARCH_INT

#include <ax_type.h>

enum mte_arch : u32{
	MTE_ARCH_MIN,
	INTEL64 = 646 ^ (1UL << 31), // ("intel64" ascii sum) ^ (0x80000000)
	MIPS32 = 542 ^ (1UL << 31), // ("mips32" ascii sum) ^ (0x80000000)
	MTE_ARCH_MAX
};
enum mte_syn : u32{
	MTE_SYN_MIN,
	SYN_INTEL = 1,
	SYN_ATNT = 2,
	MTE_SYN_MAX
};
enum mte_prior : u8{
	MTE_PRIOR_MIN,
	PRIOR_MIN,
	PRIOR_LOW,
	PRIOR_MOD,
	PRIOR_MED,
	PRIOR_HIGH,
	PRIOR_VERY_HIGH,
	PRIOR_REAL,
	PRIOR_MAX,
	MTE_PRIOR_MAX
};

#endif // !defined(MTE_ARCH_INT)

