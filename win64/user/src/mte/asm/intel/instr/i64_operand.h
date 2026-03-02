#if !defined(MTE_I64_OPERAND_INT)
#define MTE_I64_OPERAND_INT

#include <ax_type.h>

// Reduced operand count
#define I64_RED_OP_COUNT 3

// 4 operand count
#define I64_MAX_OP_COUNT 4

/*
 	TODO:
		- DEPRECATE I64_EXT AND I64_SIB_EXT
*/
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
			0x7fffffff
			2340
	*/
	I64_IMM 		= 0x04,
 	/*
	 	Extended operand flag

		I64_REG | I64_EXT Example:
			r8
			r14

		I64_MEM | I64_EXT Example:
			[r8]

		I64_MEM | I64_SIB | I64_EXT Example:
			[r8+rax*2]

	*/
	I64_EXT 		= 0x08,
 	/*
		Memory scale index base (SIB) [base + index * scale]

		Example:
			[rax+rbx*1]
			[rax+rbx*2]
			[rbx+rdx*8]

		IMPORTANT:
			This flag DOES NOT extend [base] nor [index]
			To extend the [base] use I64_EXT
			To extend the [index] use I64_SIB_EXT
	*/
	I64_SIB 		= I64_MEM | 0x10,
 	/*
		Memory scale index base (SIB) [base + index * scale] with extended [index] register

		Example:
			[rax+r9*2]
			[rbx+r10*8]

		IMPORTANT:
			This flag DOES NOT extend [base]
			To extend the base use I64_EXT
	*/
	I64_SIB_EXT 		= I64_SIB | 0x20,
 	/*
	 	Memory displacement, can either be displacement for just memory operand
		or
		can displace SIB [index] field

		Either way displacement is exclusive to memory addressing operands.

		I64_DISP8 Example:
			[0x10]
			[0x80]
			[rax+0x10]

		I64_SIB | I64_DISP8 Example:
			[rbx+0x10*8]
			[rax+rbx*2+0x20]
	*/
	I64_DISP8 		= I64_MEM | 0x40,
 	/*
	 	Memory displacement, can either be displacement for just memory operand
		or
		can displace SIB [index] field

		Either way displacement is exclusive to memory addressing operands.

		I64_DISP32 Example:
			[0xffff]
			[0x80000000]

		I64_SIB | I64_DISP32 Example:
			[rbx+0x1000*8]
			[rax+0x40000000*2]
			[rdx+rcx*1+0xffffffff]
	*/
	I64_DISP32 		= I64_MEM | 0x80,
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

enum i64_operand_id : u8{
	/*
	 	[0 - 7] -> Base registers
	 	[8 - 15] -> Extended registers
	 	[16 - x] -> Numerical type
	*/
	I64_rAX = 0b000,
	I64_rCX = 0b001,
	I64_rDX = 0b010,
	I64_rBX = 0b011,
	I64_rSP = 0b100,
	I64_rBP = 0b101,
	I64_rSI = 0b110,
	I64_rDI = 0b111,
	I64_R8  = 0b1000,
	I64_R9  = 0b1001,
	I64_R10 = 0b1010,
	I64_R11 = 0b1011,
	I64_R12 = 0b1100,
	I64_R13 = 0b1101,
	I64_R14 = 0b1110,
	I64_R15 = 0b1111,
	I64_SIB_1 = BIT(4), 		// Ex: ([rax + rbx * 1])
	I64_SIB_2 = BIT(4) | 1, 	// Ex: ([rax + rbx * 2])
	I64_SIB_4 = BIT(4) | 2, 	// Ex: ([rax + rbx * 4])
	I64_SIB_8 = BIT(4) | 3, 	// Ex: ([rax + rbx * 8])
};

typedef struct _i64_operand_desc{
	enum i64_operand_type  	type;
	enum i64_operand_width 	width;
} i64_operand_desc;
typedef struct _i64_operand{
	i64_operand_desc	desc;
	/*
		Identifier of the base register/value.

		Example for SIB addressing:
			[rax + rbx] -> id = rAX
			[rdx + rcx * 10] -> id = rDX

		Example for registers:
			rax -> id = rAX

		Example for immediate 16/32 bits:
			0xffff -> id = I64_INT_16
			0xffff -> id = I64_INT_32

		IMPORTANT!!!

		In case of immediates and memory accessing,
		value and additional information is stored in the [value] field

		If [desc.type] & I64_REG:
			- [value] == ZERO
		If [desc.type] & I64_IMM:
			- [value] == Field width value
		If [desc.type] & I64_MEM:
			- [value] == [struct i64_operand_mem]

	*/
	enum i64_operand_id	id;
	u64			value;
} i64_operand;

struct i64_operand_mem{
	/*
	 	SIB Exclusive

		IMPORTANT!!!
			- ID of the operand descriptor is the [SIB.base]
	*/
	enum i64_operand_id	scale_id;
	enum i64_operand_id	index_id;
	/*
	 	Displacement Exclusive
	*/
	u32			disp;
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

_inline_force bool _i64_operand_cmp(
	i64_operand_desc a,
	i64_operand_desc b
){
	// Check type and width
	return (a.type & b.type) && _i64_width_check(a.width, b.width);
}

#endif // !defined(MTE_I64_OPERAND_INT)

