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
enum x86_64_type : u8{
	t
};

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

_inline_force enum x86_64_type _x86_64_check_type(
	_in x86_64_mte_raw_instr	instr
){
	u8 i = 0;
	u8 legacy_pref = 0;
	enum x86_64_rex rex_pref = 0;

	u8 opcode_len = 0;
	bool opcode_ext = false;

	switch(instr[i]){
	// Extended opcode (additional opcode byte)
	case 0xf2:
	case 0xf3:
	case 0x66:
		opcode_ext = true;
	case 0xf0:
	case 0x2e:
	case 0x36:
	case 0x3e:
	case 0x26:
	case 0x64:
	case 0x65:
	case 0x67:
		i++;
		legacy_pref = instr[i];
		io_str(u"legacy");
		break;
	default:
		legacy_pref = 0;
		break;
	}

	bool is_rex = 
		(instr[i] & (nb_mask(4) << 4)) == (REX_LABEL << 4);
	rex_pref = instr[i] & nb_mask(4);

	// Multi-byte opcode (2 or 3 bytes)
	bool is_multi = (is_rex 
		? instr[++i] == 0x0f // With rex check one byte further
		: instr[i] == 0x0f); // Without rex check immediately

	i++;

	// Identify opcode byte length
	if (is_multi && !(instr[i] == 0x38 || instr[i] == 0x3a)){
		opcode_len = 2 + opcode_ext; // 3 if (legacy_pref == f2H or f1H or 66H)
	}else if (instr[i] == 0x38 || instr[i] == 0x3a){
		opcode_len = 3 + opcode_ext; // 4 if (legacy_pref == f2H or f1H or 66H)
	}else{
		opcode_len = 1;
	}
	io_i64(opcode_len);
	return 0;
}

#endif // !defined(AX_X86_64_INT)

