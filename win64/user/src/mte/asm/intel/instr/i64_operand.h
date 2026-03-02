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
			[r8]
	*/
	I64_MEM 		= 0x01,
	/*
	 	Register operand flag (Should not be used with I64_MEM or I64_IMM)

		Example:
			rax
			eax
			al
			r8d
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
		Memory scale index base (SIB) [base + index * scale]

		Example:
			[r10+rbx*1]
			[rax+rbx*2]
			[rbx+r8*8]
	*/
	I64_SIB 		= I64_MEM | 0x10,
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

#define I64_OP_REG 		0b000 << 5
#define I64_OP_SIB_SCALE 	0b010 << 5
#define I64_OP_IMM 		0b011 << 5

#define I64_OP_EXT 		0b100 << 5

/*
	(SHOULD NOT BE USED EXTERNALY)
 	Initialize operand with value (v) and type (i) 

	(i) is limited to 3 (shifted left by 5) bits
	(v) is limited to 5 bits
*/
#define _I64_OPR_I(i, v) \
	(((i) | (v)) & 0xff)

/*
	[0 - 7] -> Base registers
	[8 - 15] -> Extended registers
	[16 - x] -> Numerical type
*/
enum i64_operand_id : u8{
	I64_rAX 	= _I64_OPR_I(I64_OP_REG, 0b0000),
	I64_rCX 	= _I64_OPR_I(I64_OP_REG, 0b0001),
	I64_rDX 	= _I64_OPR_I(I64_OP_REG, 0b0010),
	I64_rBX 	= _I64_OPR_I(I64_OP_REG, 0b0011),
	I64_rSP 	= _I64_OPR_I(I64_OP_REG, 0b0100),
	I64_rBP		= _I64_OPR_I(I64_OP_REG, 0b0101),
	I64_rSI		= _I64_OPR_I(I64_OP_REG, 0b0110),
	I64_rDI 	= _I64_OPR_I(I64_OP_REG, 0b0111),
	I64_r8		= _I64_OPR_I(I64_OP_EXT | I64_OP_REG, 0b1000),
	I64_r9		= _I64_OPR_I(I64_OP_EXT | I64_OP_REG, 0b1001),
	I64_r10 	= _I64_OPR_I(I64_OP_EXT | I64_OP_REG, 0b1010),
	I64_r11 	= _I64_OPR_I(I64_OP_EXT | I64_OP_REG, 0b1011),
	I64_r12 	= _I64_OPR_I(I64_OP_EXT | I64_OP_REG, 0b1100),
	I64_r13 	= _I64_OPR_I(I64_OP_EXT | I64_OP_REG, 0b1101),
	I64_r14 	= _I64_OPR_I(I64_OP_EXT | I64_OP_REG, 0b1110),
	I64_r15 	= _I64_OPR_I(I64_OP_EXT | I64_OP_REG, 0b1111),

	// Ex: [rax + rbx * 1]
	I64_SIB_1 	= _I64_OPR_I(I64_OP_SIB_SCALE, BIT(4)), 	
	// Ex: [rax + rbx * 2]
	I64_SIB_2 	= _I64_OPR_I(I64_OP_SIB_SCALE, BIT(4) | 1),
	// Ex: [rax + rbx * 4]
	I64_SIB_4 	= _I64_OPR_I(I64_OP_SIB_SCALE, BIT(4) | 2),
	// Ex: [rax + rbx * 8]
	I64_SIB_8 	= _I64_OPR_I(I64_OP_SIB_SCALE, BIT(4) | 3),

	I64_IMM_8 	= _I64_OPR_I(I64_IMM, BIT(4) | 4),
	I64_IMM_16 	= _I64_OPR_I(I64_IMM, BIT(4) | 5),
	I64_IMM_32 	= _I64_OPR_I(I64_IMM, BIT(4) | 6),
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

