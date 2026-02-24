#include <ax_type.h>

/*
 	Naming of individual registers follows 16-bit width naming.
	These do not include XMM registers! ()
	Example:
		rBP for 8-bit would be CH
		rDI for 8-bit would be BH
		and so on...
*/
enum i64_reg : u8{
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
};

