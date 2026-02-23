#if !defined(MTE_I64_OPERAND_INT)
#define MTE_I64_OPERAND_INT

#include <ax_type.h>

// Reduced operand count
#define I64_RED_OP_COUNT 3

// 4 operand count
#define I64_MAX_OP_COUNT 4

enum i64_operand_type : u8{
	I64_MEM 		= 1,
	I64_REG 		= 2, // MODRM Present
	I64_REG_MEM 	= I64_REG | I64_MEM,
	I64_IMM 		= 8,
	I64_REG_MEM_IMM 	= I64_IMM | I64_REG_MEM,
};
enum i64_operand_width : u8{
	W8 	= 3,
	W16 	= 4,
	W32 	= 5,
	W64 	= 6,
	/*
	   	Each enum value is the power of 2 to get bitwidth
		(2^3 = 8, 2^4 = 16, etc...)
	 	
	 	Reserved for later (SIMD support)
	W256 	= 8,
	W128 	= 7,
	W512 	= 9,
	*/
};
_inline_force bool _i64_width_check(
	enum i64_operand_width a,
	enum i64_operand_width b
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

typedef struct _i64_operand_desc{
	enum i64_operand_type  	type;
	enum i64_operand_width 	width;
} i64_operand_desc;
typedef struct _i64_operand{
	i64_operand_desc	desc;
	u64			value;
} i64_operand;

_inline_force bool _i64_operand_cmp(
	i64_operand_desc a,
	i64_operand_desc b
){
	// Check type and width
	return (a.type & b.type) && _i64_width_check(a.width, b.width);
}

#endif // !defined(MTE_I64_OPERAND_INT)

