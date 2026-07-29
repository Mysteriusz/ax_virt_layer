#if !defined(MTE_ARCH_INT)
#define MTE_ARCH_INT

#include <ax_type.h>

/*
	These define what form most instructions require in this ISA
	Although this can still be overrided when it comes to exceptions

	Exception example for INTEL64:

		- imul rax, rbx, 8

		Which is:

		- rax = rbx * 8

		This violates how generally Intel64
		orders operands
*/
enum mte_isa_form : u8{
	MTE_ORDER_MIN,
	MTE_ISA_TWO_OP = 1, // opcode r0, r1
	MTE_ISA_THREE_OP = 2, // r0 = opcode r1, r2
	MTE_ORDER_MAX
};
enum mte_arch : u32{
	MTE_ARCH_MIN,
	INTEL64 = (MTE_ISA_TWO_OP << 24) | 1,
	MIPS32 = (MTE_ISA_THREE_OP << 24) | 2,
	MTE_ARCH_MAX
};
#define MTE_ARCH_ISA_FORM(arch) ((arch >> 24) & 0xff)

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

