#if !defined(MTE_X86_64_INT)
#define MTE_X86_64_INT

#include <ax_type.h>
#include "mte/asm/decode_u64.h"
#include "mte/cpu.h"
#include "mte/ir.h"

static struct cpu_reg_map _x86_64_cpu_reg_map = {
	.gpr_count = 16, // 16 gprs registers rax-r15
	.reg_width = 64, // 64-Bit architecture
	.root = (reg64[16]){}
};

/*
 	IMPORTANT!

 	Currently all translations are expected to be in IA-32E with 64-bit mode.
*/

typedef u8* x86_64_mte_raw_instr; // Unknown length instruction (up to 15 bytes)
#define init_x86_64_mte_raw_instr(...) ((x86_64_mte_raw_instr)((u8[15]){__VA_ARGS__}))

/*	
 	x86_64 instruction breakdown
*/


/*
 	MIPS32 instruction info retrieval
*/

#define REX_LABEL 0b0100

/*
 	Any figure labels refer to Intels x86_64 instruction set manual:

	Intel® 64 and IA-32 Architectures Software Developer's Manual Combined Volumes 2A, 2B, 2C, and 2D: Instruction Set Reference, A- Z
*/
enum x86_64_rex : u8{
	REX_B = 0b0001, // Figure 2-7 
};
#define x86_64_rex_w(r) 	(r & (1 << 3))
#define x86_64_rex_r(r) 	(r & (1 << 2))
#define x86_64_rex_x(r) 	(r & (1 << 1))
#define x86_64_rex_b(r) 	(r & (1 << 0))

_inline_force bool _x86_64_legacy_ext(
	_in x86_64_mte_raw_instr	instr
){
	// Legacy opcode-extending prefix check
	switch(instr[0]){
	case 0xf2:
	case 0xf3:
	case 0x66:
		return true;
	default:
		return false;
	}
}
_inline_force u8 _x86_64_legacy(
	_in x86_64_mte_raw_instr	instr
){
	// Legacy prefix check
	switch(instr[0]){
	case 0xf2:
	case 0xf3:
	case 0x66:
	case 0xf0:
	case 0x2e:
	case 0x36:
	case 0x3e:
	case 0x26:
	case 0x64:
	case 0x65:
	case 0x67:
		return instr[0];
	default:
		return 0;
	}
}
_inline_force u8 _x86_64_rex(
	_in x86_64_mte_raw_instr	instr
){
	u8 i = (_x86_64_legacy(instr) != 0);
	switch(instr[i]){
	case 0x40 ... 0x4f:
		return ((((instr[i] >> 4) & nb_mask(4)) == REX_LABEL)
			? instr[i]
			: 0);
	default:
		return 0;
	}
}
_inline_force bool _x86_64_rex_ext(
	_in x86_64_mte_raw_instr	instr
){
	u8 i = (_x86_64_legacy(instr) != 0);
	switch(instr[i]){
	case 0x40 ... 0x4f:
		return true;
	default:
		return false;
	}
}
_inline_force u32 _x86_64_opcode(
	_in x86_64_mte_raw_instr	instr
){
	u8 i = 0;

	u8 opcode_i = 0;
	u8 opcode_len = 0;

	bool is_legacy = _x86_64_legacy_ext(instr);
	i++;

	bool is_rex = _x86_64_rex_ext(instr);
	i += is_rex;
	i++;

	// Identify opcode byte length
	if (instr[i] == 0x38 || instr[i] == 0x3a){
		opcode_len = 2 + is_rex + is_legacy; // 4 if (legacy_pref == f2H or f1H or 66H)
	}else{
		opcode_len = 1 + is_rex + is_legacy; // 3 if (legacy_pref == f2H or f1H or 66H)
	}

	u64 mask = n_mask(opcode_len);
	if (is_legacy && !is_rex){ // Edge case when we also need first (legacy) byte
		mask |= 0xff << 24; // Last byte set
	}

	// Mask and normalize the opcode
	return (*(u32*)offp(instr, opcode_i)) & mask >> (sizeof(u32) - opcode_len);
}

#endif // !defined(AX_X86_64_INT)

