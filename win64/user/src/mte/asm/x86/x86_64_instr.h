#if !defined(AX_X86_64_INSTR_INT)
#define AX_X86_64_INSTR_INT

#include <ax_type.h>
#include <ax_io.h>
#include "mte/asm/x86/x86_64_modrm.h"

/*
 	IMPORTANT!

 	Currently all translations are expected to be in IA-32E with 64-bit mode.
*/

typedef u8 const* x86_64_mte_raw_instr; // Unknown length instruction (up to 15 bytes)
#define init_x86_64_mte_raw_instr(...) ((x86_64_mte_raw_instr)((const u8[15]){__VA_ARGS__}))

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
#define rex_w(r) 	(bool)(r & (1 << 3))
#define rex_r(r) 	(bool)(r & (1 << 2))
#define rex_x(r) 	(bool)(r & (1 << 1))
#define rex_b(r) 	(bool)(r & 1)

#define info_r(i) 	(bool)(i & (1 << 2))
#define info_l(i) 	(bool)(i & (1 << 1))
#define info_e(i) 	(bool)(i & 1)

// Opcode intermediate
typedef struct _x86_64_opcode{
	const u32 	val;
	const u8 	len;
	const u8 	info; // 00000RLE (is rex | is legacy | is legacy extension)
	const u8	legacy; // Legacy prefix value
	const u8	rex; // Rex prefix value
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
	case 0x40 ... 0x4f: // Valid rex byte
		return instr[i];
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
 	// Base extension checks
	if (is_legacy && instr[is_rex + 1] == 0x0f){
		ext = true;
	}else if(instr[is_rex] == 0x0f){
		ext = true;
	}

	switch(instr[is_legacy + is_rex + 1]){
 	// Additional extension checks
	case 0x38:
	case 0x3a:
		return 2 + is_legacy + ext;
	default:
		return 1 + is_legacy + ext;
	}
}
_inline_force const x86_64_opcode _x86_64_get_opcode(
	_in const x86_64_mte_raw_instr	instr
){
	u8 opcode_len = 0;

	u8 rex = _x86_64_get_rex(instr);
	u8 legacy = _x86_64_get_legacy(instr); // 0 (false) if legacy isn`t present

	bool is_rex = (rex != 0);
	bool is_legacy = (legacy != 0); // 0 (false) if legacy isn`t present
	bool is_legacy_ext = _x86_64_legacy_ext(instr); // Is legacy prefix a part of the opcode

	u8 info = is_rex << 2 | is_legacy << 1 | is_legacy_ext;

	opcode_len = _x86_64_opcode_len(instr, is_legacy_ext, is_rex);
	u64 mask = n_mask(opcode_len);

	if (is_rex && is_legacy_ext){
		return (const x86_64_opcode){
			.val = (((*(u32*)offp(instr, 1)) & mask) & ~0xff) | is_legacy, 
			.len = opcode_len,
			.info = info,
			.legacy = legacy,
			.rex = rex
		}; // From after rex with legacy byte
	}else{
		return (const x86_64_opcode){
			.val = (((*(u32*)offp(instr, is_rex)) & mask)), 
			.len = opcode_len,
			.info = info,
			.legacy = legacy,
			.rex = rex
		}; // From rex or first byte
	}
}

/*
 	SIB follows the MODRM byte:
	If (mod == 00 OR mod == 01 OR mod == 10) AND r/m == 100

	If mod == 00 AND r/m == 101 then:
		Following the MODRM byte is the 4 byte disp (u32)

	If mod == 01 then:
		disp is encoded in 1 byte after SIB (u8)

	If mod == 10 then:
		disp is encoded in 4 bytes after SIB (u32)
*/
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

	// Return byte at offset of opcode + legacy + is_rex
	return instr[opcode.len + info_r(opcode.info)];
}

/*
	[index * scale + base]

	scale:
	00 -> 1
	01 -> 2
	10 -> 4
	11 -> 8

	index:
	000 -> EAX
	001 -> ECX
	010 -> EDX
	011 -> EBX
	100 -> 0
	101 -> EBP
	110 -> ESI
	111 -> EDI

	base:
	(1 ... 7) (excluding 5)
	base == 5 indicates the displacement is placed after the SIB byte.
	It`s byte length is dependant on the mod AND r/m fields of the MODRM.
*/
#define sib_scale(m) ((u8)(m) >> 6)
#define sib_index(m) (((u8)(m) >> 3) & 0x7)
#define sib_base(m) ((u8)(m) & 0x7)

/*
 	Modrm checking sib read.
*/
_inline_force u8 _x86_64_get_sib(
	_in x86_64_opcode		opcode,
	_in u8				modrm,
	_in x86_64_mte_raw_instr	instr
){
	if (modrm_mod(modrm) == 0b11
	|| modrm_rm(modrm) != 0b100){
		return 0;
	}

	// Return byte at offset of opcode + legacy + is_rex + modrm
	return instr[opcode.len + info_r(opcode.info) + 1];
}

_inline_force u32 _x86_64_get_disp_sib(
	_in x86_64_opcode		opcode,
	_in u8				sib_i, // index of instr where sib is located
	_in u8				modrm,
	_in x86_64_mte_raw_instr	instr
){
	// Invalid modrm byte
	if (modrm_mod(modrm) == 0b11
	|| modrm_rm(modrm) != 0b100){
		return 0;
	}
	u8 sib = instr[sib_i];
	
	// Sib is placed in either 1 or 4 bytes after sib
	switch(modrm_mod(modrm)){
	case 0b00:
		if (sib_base(sib) == 5){
			return *(u32*)offp(instr, sib_i + 1);
		}
		return 0;
	case 0b01:
		return instr[sib_i + 1];
	case 0b10:
		// Swap from array (Big endian) back to (Little endian)
		return *(u32*)offp(instr, sib_i + 1);
	default:
		return 0;
	}
}
_inline_force u32 _x86_64_get_disp(
	_in x86_64_opcode		opcode,
	_in u8				modrm,
	_in x86_64_mte_raw_instr	instr
){
	if (modrm_mod(modrm) == 0b11){
		return 0;
	}

	u8 modrm_i = opcode.len + info_r(opcode.info); // opcode length + is_rex
	// SIB byte present
	if (modrm_rm(modrm) == 0b100){
		return _x86_64_get_disp_sib(
			opcode,
			modrm_i + 1,
			modrm,
			instr);
	}

	// No SIB present
	switch(modrm_mod(modrm)){
	case 0b10:
		return *(u32*)offp(instr, modrm_i + 1);
	case 0b00: 
		if (modrm_rm(modrm) == 0b101){
			return *(u32*)offp(instr, modrm_i + 1);
		}
		return 0;
	case 0b01:
		return instr[modrm_i + 1];
	default:
		return 0;
	}
}

#endif // !defined(AX_X86_64_INSTR_INT)

