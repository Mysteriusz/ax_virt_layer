#include "intel/instr/i64_operand.h"

enum i64_operand_id I64_REG_TO_OPERAND[16] = {
	[0b0000] = I64_rAX,
    	[0b0001] = I64_rCX,
    	[0b0010] = I64_rDX,
    	[0b0011] = I64_rBX,
    	[0b0100] = I64_rSP,
    	[0b0101] = I64_rBP,
    	[0b0110] = I64_rSI,
    	[0b0111] = I64_rDI,
    	[0b1000] = I64_r8,
    	[0b1001] = I64_r9,
    	[0b1010] = I64_r10,
    	[0b1011] = I64_r11,
    	[0b1100] = I64_r12,
    	[0b1101] = I64_r13,
    	[0b1110] = I64_r14,
    	[0b1111] = I64_r15,
};

