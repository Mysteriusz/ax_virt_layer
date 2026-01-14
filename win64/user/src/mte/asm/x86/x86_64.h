#if !defined(MTE_X86_64_INT)
#define MTE_X86_64_INT

#include <ax_type.h>
#include "mte/asm/decode_u64.h"
#include "mte/cpu.h"
#include "mte/ir.h"
#include "mte/asm/x86/x86_64_modrm.h"

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

	AVX Instruction set not supported yet!
	x87 FPU Instruction set not supported yet!
*/
enum x86_64_rex : u8{
	REX_B = 0b0001, // Figure 2-7 
};
#define x86_64_rex_w(r) 	(r & (1 << 3))
#define x86_64_rex_r(r) 	(r & (1 << 2))
#define x86_64_rex_x(r) 	(r & (1 << 1))
#define x86_64_rex_b(r) 	(r & (1 << 0))

// Opcode intermediate
typedef struct _x86_64_opcode{
	u32 		val;
	u8 		len;
} x86_64_opcode _align(8);

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
_inline_force u8 _x86_64_get_legacy(
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
_inline_force u8 _x86_64_get_rex(
	_in x86_64_mte_raw_instr	instr
){
	u8 i = (_x86_64_get_legacy(instr) != 0);
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
	u8 i = (_x86_64_get_legacy(instr) != 0);
	switch(instr[i]){
	case 0x40 ... 0x4f:
		return true;
	default:
		return false;
	}
}
_inline_force u8 _x86_64_opcode_len(
	_in x86_64_mte_raw_instr	instr,
	_in bool 			is_legacy,
	_in bool 			is_rex
){
	bool ext = false;
	if (is_legacy && instr[is_rex + 1] == 0x0f){
		ext = true;
	}else if(instr[is_rex] == 0x0f){
		ext = true;
	}

	switch(instr[is_legacy + is_rex + 1]){
	case 0x38:
	case 0x3a:
		return 2 + is_legacy + ext;
	default:
		return 1 + is_legacy + ext;
	}
}
_inline_force const x86_64_opcode _x86_64_get_opcode(
	_in x86_64_mte_raw_instr	instr
){
	u8 opcode_len = 0;

	bool is_rex = _x86_64_rex_ext(instr);
	u8 is_legacy = _x86_64_get_legacy(instr); // 0 (false) if legacy isn`t present
	bool is_legacy_ext = _x86_64_legacy_ext(instr); // Is legacy prefix a part of the opcode

	opcode_len = _x86_64_opcode_len(instr, is_legacy_ext, is_rex);
	u64 mask = n_mask(opcode_len);

	if (is_rex && is_legacy_ext){
		return (const x86_64_opcode){
			.val = (((*(u32*)offp(instr, 1)) & mask) & ~0xff) | is_legacy, 
			.len = opcode_len
		}; // From after rex with legacy byte
	}else{
		return (const x86_64_opcode){
			.val = (((*(u32*)offp(instr, is_rex)) & mask)), 
			.len = opcode_len
		}; // From rex or first byte
	}
}

#define modrm_mod(m) ((u8)(m) >> 6)
#define modrm_reg(m) (((u8)(m) >> 3) & 0x7)
#define modrm_rm(m) ((u8)(m) & 0x7)

/*
 	Opcode checking modrm read.
*/
_inline_force u8 _x86_64_get_modrm(
	_in x86_64_opcode		opcode,
	_in x86_64_mte_raw_instr	instr
){
	if (!_x86_64_modrm_check(opcode.val)){
		return 0;
	}

	bool is_rex = _x86_64_rex_ext(instr);
	// Return byte at offset of opcode + legacy + is_rex
	return instr[opcode.len + is_rex];
}

/*
 	Modrm checking sib read.
*/
_inline_force u8 _x86_64_get_sib(
	_in x86_64_opcode		opcode,
	_in x86_64_mte_raw_instr	instr
){
	u8 modrm = _x86_64_get_modrm(opcode, instr);
	if (modrm_mod(modrm) == 0x2
	|| modrm_rm(modrm) != 0x4){
		return 0;
	}

	bool is_rex = _x86_64_rex_ext(instr);
	// Return byte at offset of opcode + legacy + is_rex + modrm
	return instr[opcode.len + is_rex + 1];
}

#endif // !defined(AX_X86_64_INT)

