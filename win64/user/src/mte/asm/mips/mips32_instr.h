#if !defined(MTE_MIPS32_INSTR_INT) 
#define MTE_MIPS32_INSTR_INT

#include <ax_type.h>

enum mips32_instr_type{
	R = 1, // register to register instrucitons
	I = 2, // immidiate to register instructions
	J = 3, // memory read instructions
};

typedef u32 mips32_mte_raw_instr;
/*	
 	mips32 instruction breakdown
*/
enum mips32_reg{
	R0, // Zero (0)
	R1, // Reserved for asm (at)
	R2, R3, // Results (v0-v1)
	R4, R5, R6, R7, // Arguments (a0-a3)
	R8, R9, R10, R11, R12, R13, R14, R15, // Temporary (t0-t7)
	R16, R17, R18, R19, R20, R21, R22, R23, // Saved (s0-s7)
	R24, R25, // Not saved (t8-t9)
	R26, R27, // OS reserved (k0-k1)
	R28, // Global pointer (gp)
	R29, // Stack pointer (sp)
	R30, // Frame pointer (fp/s8)
	R31 // Return address (ra)
};

typedef u8 mips32_opcode;
#define MIPS32_OPCODE_INVALID (mips32_opcode)0b111111

/*
 	MIPS32 instruction info retrieval
*/

#define MIPS32_FUNCT_SHIFT 0
#define MIPS32_SHAMT_SHIFT 0
#define MIPS32_RD_SHIFT 11
#define MIPS32_RS_SHIFT 21
#define MIPS32_RT_SHIFT 16
#define MIPS32_OPCODE_SHIFT 26

#define _mips32_funct(instr)	(instr & 0x3f)
#define _mips32_shamt(instr)	((instr >> MIPS32_SHAMT_SHIFT) & 0x1f)
#define _mips32_rd(instr)	((instr >> MIPS32_RD_SHIFT) & 0x1f)
#define _mips32_rt(instr)	((instr >> MIPS32_RT_SHIFT) & 0x1f)
#define _mips32_rs(instr)	((instr >> MIPS32_RS_SHIFT) & 0x1f)
#define _mips32_opcode(instr)	((instr >> MIPS32_OPCODE_SHIFT) & 0x3f)

_inline_force enum mips32_instr_type _mips32_get_instr_type(
	_in mips32_mte_raw_instr	instr
){
	if (_mips32_opcode(instr) == 0){
		return R;
	}else if(_mips32_opcode(instr) != 2 && _mips32_opcode(instr) != 3){
		return I;
	}else{
		return J;
	}
	return 0;
}

#endif // !defined(MTE_MIPS32_INSTR_INT)

