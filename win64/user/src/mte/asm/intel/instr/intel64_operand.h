#if !defined(MTE_INTEL64_OPERAND_INT)
#define MTE_INTEL64_OPERAND_INT

#include <ax_type.h>

// Reduced operand count
#define INTEL64_RED_OP_COUNT 3

// 4 operand count
#define INTEL64_MAX_OP_COUNT 4

enum intel64_operand_type : u8{
	INTEL64_MEM 		= 1,
	INTEL64_REG 		= 2,
	INTEL64_REG_MEM 	= INTEL64_REG | INTEL64_MEM,
	INTEL64_IMM 		= 8,
	INTEL64_REG_MEM_IMM 	= INTEL64_IMM | INTEL64_REG_MEM,
};
enum intel64_operand_width : u8{
	/*
	   	Each enum value is the power of 2 to get bitwidth
		(2^3 = 8, 2^4 = 16, etc...)
	 	
	 	Reserved for later (SIMD support)
	W512 	= 9,
	W256 	= 8,
	W128 	= 7,
	*/
	W64 	= 6,
	W32 	= 5,
	W16 	= 4,
	W8 	= 3,
};
_inline_force bool _intel64_width_check(
	enum intel64_operand_width a,
	enum intel64_operand_width b
){
	switch(a){
	case W64:
		return (b >= W16) && (b <= W64);
	case W32:
		return (b >= W16) && (b <= W32);
	case W16:
		return (b == W16);
	case W8:
		return (b == W8);
	default:
		return false;
	}
}

struct intel64_operand_desc{ _align(2)
	enum intel64_operand_type  	type;
	enum intel64_operand_width 	width;
};
typedef struct _intel64_operand{
	struct intel64_operand_desc	desc;
	u64				value;
} intel64_operand;

_inline_force bool _intel64_operand_cmp(
	struct intel64_operand_desc a,
	struct intel64_operand_desc b
){
	// Check type and width
	return (a.type & b.type) && _intel64_width_check(a.width, b.width);
}

#endif // !defined(MTE_INTEL64_OPERAND_INT)

