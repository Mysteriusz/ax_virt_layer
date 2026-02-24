#if !defined(MTE_I64_OPERAND_INT)
#define MTE_I64_OPERAND_INT

#include <ax_type.h>

// Reduced operand count
#define I64_RED_OP_COUNT 3

// 4 operand count
#define I64_MAX_OP_COUNT 4

enum i64_operand_type : u8{
	/*
	 	Memory operand flag (Should not be used with I64_REG or I64_IMM)

		Example:
			[rax]
			[eax]

		IMPORTANT:
			For it to be used with extended registers I64_EXT or I64_SIB_EXT should be used
	*/
	I64_MEM 		= 0x01,
	/*
	 	Register operand flag (Should not be used with I64_MEM or I64_IMM)

		Example:
			rax
			eax
			al

		IMPORTANT:
			For it to be used with extended registers I64_EXT or I64_SIB_EXT should be used
	*/
	I64_REG 		= 0x02,
	/*
	 	Immediate operand flag (Should not be used with I64_MEM or I64_REG)

		Example:
			rax
			eax
			al

		IMPORTANT:
			For it to be used with extended registers I64_EXT or I64_SIB_EXT should be used
	*/
	I64_IMM 		= 0x04,
 	/*
	 	Extended operand flag

		I64_MEM | I64_EXT Examples:
			[r8]
			[r8+rax*2]

		I64_REG | I64_EXT Examples:
			r8
			r14
	*/
	I64_EXT 		= 0x08,
 	/*
		For memory scale index base (SIB) [base + index * scale]
		For cases without [index] as an extended register this should be false

		Example:
			- [rax+r9*2]
			- [rbx+r10*8]

		IMPORTANT:
			This flag DOES NOT extend [base]
			To extend the base use I64_EXT
	*/
	I64_SIB_EXT 		= 0x10,
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

typedef struct _i64_operand_desc{
	enum i64_operand_type  	type;
	enum i64_operand_width 	width;
} i64_operand_desc;
typedef struct _i64_operand{
	i64_operand_desc	desc;
	u64			value;
} i64_operand;

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

_inline_force bool _i64_operand_cmp(
	i64_operand_desc a,
	i64_operand_desc b
){
	// Check type and width
	return (a.type & b.type) && _i64_width_check(a.width, b.width);
}

#endif // !defined(MTE_I64_OPERAND_INT)

