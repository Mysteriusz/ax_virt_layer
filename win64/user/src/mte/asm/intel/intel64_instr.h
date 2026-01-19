#if !defined(AX_INTEL64_INSTR_INT)
#define AX_INTEL64_INSTR_INT

#include <ax_type.h>
#include <ax_io.h>

#include "mte/asm/intel/tables/intel64_qtables.h"
#include "mte/asm/intel/tables/intel64_modrm.h"
#include "mte/asm/intel/tables/intel64_immd.h"

/*
 	IMPORTANT!

 	Currently all translations are expected to be in IA-32E with 64-bit mode.
*/

typedef u8 const* intel64_mte_raw_instr; // Unknown length instruction (up to 15 bytes)
#define init_intel64_mte_raw_instr(...) ((intel64_mte_raw_instr)((const u8[15]){__VA_ARGS__}))

/*	
 	intel64 instruction breakdown
*/


/*
 	MIPS32 instruction info retrieval
*/

#define REX_LABEL 0b0100

/*
 	Any figure labels refer to Intels intel64 instruction set manual:

	Intel® 64 and IA-32 Architectures Software Developer's Manual Combined Volumes 2A, 2B, 2C, and 2D: Instruction Set Reference, A- Z

	AVX Instruction set not supported yet!
	x87 FPU Instruction set not supported yet!
*/
#define rex_w(r) 	(bool)(r & (1 << 3))
#define rex_r(r) 	(bool)(r & (1 << 2))
#define rex_x(r) 	(bool)(r & (1 << 1))
#define rex_b(r) 	(bool)(r & 1)

typedef struct _packed _intel64_opcode_info{
	u8 reserved : 4; // 0000
	u8 x : 1; // Is extended by 0FH
	u8 r : 1; // Has rex
	u8 l : 1; // Has legacy
	u8 e : 1; // Has legacy as opcode extension (66H, F2H, F3H)
} intel64_opcode_info;

// Opcode intermediate
typedef struct _packed _intel64_opcode{
	const u32 			val;
	const u8 			len;
	const intel64_opcode_info 	info; // 0000XRLE (is opcode extension (0x0f) | is rex | is legacy | is legacy extension)
	const u8			legacy; // Legacy prefix value
	const u8			rex; // Rex prefix value
} intel64_opcode _align(8);

_inline_force bool _intel64_legacy_ext(
	_in intel64_mte_raw_instr	instr
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
_inline_force u8 _intel64_get_legacy(
	_in intel64_mte_raw_instr	instr
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
_inline_force u8 _intel64_get_rex(
	_in intel64_mte_raw_instr	instr
){
	u8 i = (_intel64_get_legacy(instr) != 0);
	switch(instr[i]){
	case 0x40 ... 0x4f: // Valid rex byte
		return instr[i];
	default:
		return 0;
	}
}
_inline_force bool _intel64_rex_ext(
	_in intel64_mte_raw_instr	instr
){
	u8 i = (_intel64_get_legacy(instr) != 0);
	switch(instr[i]){
	case 0x40 ... 0x4f:
		return true;
	default:
		return false;
	}
}
_inline_force u8 _intel64_opcode_len(
	_in intel64_mte_raw_instr	instr,
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

_inline_force const intel64_opcode _intel64_get_opcode(
	_in const intel64_mte_raw_instr	instr
){
	u8 opcode_len = 0;

	u8 rex = _intel64_get_rex(instr);
	u8 legacy = _intel64_get_legacy(instr); // 0 (false) if legacy isn`t present

	// Initialize information structure
	const intel64_opcode_info info = (const intel64_opcode_info){ 
		.x = instr[(legacy != 0) + (rex != 0)] == 0x0f,
		.r = (rex != 0),
		.l = (legacy != 0),
		.e = _intel64_legacy_ext(instr),
	};

	opcode_len = _intel64_opcode_len(instr, info.l, info.r);
	u64 mask = n_mask(opcode_len);

	if (info.r && info.e){
		return (const intel64_opcode){
			.val = (((*(u32*)offp(instr, 1)) & mask) & ~0xff) | legacy,
			.len = opcode_len,
			.info = info,
			.legacy = legacy,
			.rex = rex
		}; // From after rex with legacy byte
	}else{
		return (const intel64_opcode){
			.val = (((*(u32*)offp(instr, info.r)) & mask)), 
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
 	Modrm compatilbity check for opcode with legacy prefix

	Should be optimized (remove the swtich)
*/
_inline_force bool _intel64_modrm_check(
	_in intel64_opcode	opcode
){
	// 4 byte opcode (any 4 bit opcode has MODRM byte)
	if (opcode.len == 4){
		return true;
	}
	
	// Only possiblity is 1 or 2
	if (opcode.len == 1){
		return INTEL64_MODRM_VTB(modrm_tables->l0_mask, opcode.val & 0xff);
	}
	if (opcode.len == 2){
		return INTEL64_MODRM_VTB(modrm_tables->l1_mask, (opcode.val >> 8) & 0xff);
	}

	const u64 *modrm_table = L2M_MASK_LOOKUP[opcode.val & 0x7];
	return INTEL64_MODRM_VTB(modrm_table, (opcode.val >> 16) & 0xff);
}
/*
 	Opcode checking modrm read.
*/
_inline_force u8 _intel64_get_modrm(
	_in intel64_opcode		opcode,
	_in intel64_mte_raw_instr	instr
){
	if (!_intel64_modrm_check(opcode)){
		return 0;
	}

	// Return byte at offset of opcode + legacy + is_rex
	return instr[opcode.len + opcode.info.r];
}

/*
	[index + base * scale]

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
	base == 5 indicates the displacement is the next 4 bytes.
*/
#define sib_scale(m) ((u8)(m) >> 6)
#define sib_index(m) (((u8)(m) >> 3) & 0x7)
#define sib_base(m) ((u8)(m) & 0x7)

typedef struct _packed _intel64_sib{
	// Second byte
	u8	index_ext : 1; // Extended by it`s rex X field
	u8	index : 3; 
	u8	base_ext : 1; // Extended by it`s rex B field
	u8	base : 3;
	// First byte
	u8	val;
} intel64_sib _align(8);

_inline_force const bool _intel64_sib_ext(
	_in u8				modrm
){
	return modrm_rm(modrm) == 0b100;
}
/*

	if (opcode.len != 3){
		return INTEL64_MODRM_VTB(modrm_tables->l1_mask, (opcode.val >> (8 * (opcode.len - 1))) & 0xff);
	}
 	Modrm checking sib read.
*/
_inline_force const intel64_sib _intel64_get_sib(
	_in intel64_opcode		opcode,
	_in u8				modrm,
	_in intel64_mte_raw_instr	instr
){
	if (!_intel64_sib_ext(modrm)
	|| modrm_mod(modrm) == 0b11){
		return (intel64_sib){0}; // No SIB byte
	}

	u8 sib = instr[opcode.len + opcode.info.r + 1];

	// Return byte at offset of opcode + legacy + is_rex + modrm
	return (const intel64_sib){
		.index_ext = rex_x(opcode.rex),
		.index = sib_index(sib),
		.base_ext = rex_b(opcode.rex),
		.base = sib_base(sib),
		.val = sib
	};
}

_inline_force u32 _intel64_get_disp_sib(
	_in intel64_opcode		opcode,
	_in u8				sib_i, // index of instr where sib is located
	_in u8				modrm,
	_in intel64_mte_raw_instr	instr
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
		// 4 byte displacement
		if (sib_base(sib) == 5){
			return *(u32*)offp(instr, sib_i + 1);
		}
		return 0;
	case 0b01:
		// 1 byte displacement
		return instr[sib_i + 1];
	case 0b10:
		// 4 byte displacement
		return *(u32*)offp(instr, sib_i + 1);
	default:
		return 0;
	}
}
_inline_force u8 _intel64_disp_len(
	_in intel64_opcode 		opcode,
	_in u8				modrm
){
	if (modrm_mod(modrm) == 0b11
	|| modrm_rm(modrm) != 0b100){
		return 0;
	}
	return MOD_DLEN_LOOKUP[modrm_mod(modrm)];
}

_inline_force u32 _intel64_get_disp(
	_in intel64_opcode		opcode,
	_in u8				modrm,
	_in intel64_mte_raw_instr	instr
){
	if (modrm_mod(modrm) == 0b11){
		return 0;
	}

	u8 modrm_i = opcode.len + opcode.info.r; // opcode length + is_rex
	// SIB byte present
	if (modrm_rm(modrm) == 0b100){
		return _intel64_get_disp_sib(
			opcode,
			modrm_i + 1,
			modrm,
			instr);
	}

	// No SIB present
	switch(modrm_mod(modrm)){
	case 0b00: 
		// 4 byte displacement
		if (modrm_rm(modrm) == 0b101){
			return *(u32*)offp(instr, modrm_i + 1);
		}
		return 0;
	case 0b01:
		// 1 byte displacement
		return instr[modrm_i + 1];
	case 0b10:
		// 4 byte displacement
		return *(u32*)offp(instr, modrm_i + 1);
	default:
		return 0;
	}
}
 
_inline_force u8 _intel64_immd_len(
	_in intel64_opcode	opcode,
	_in u8			modrm
){
	u8 res = 0;
	u8 sub = 1;
	u8 len = 0;

	u8 op0 = opcode.val & 0xff;

	// Only case where l0_mask cannot be found at L2I_MASK_LOOKUP
	if (opcode.len == 1){
		res = INTEL64_IMMD_VTB(immd_tables->l0_mask, op0);
		len = MASK_TO_LEN(res);

		// If submask is present calculate sub value with modrm
		if (res == 0b11
		&& op0 >= 0x7f){
			sub = immd_tables->l0_submask[op0 - 0x7f];
			len = ((sub >> (sub & 1 ? 1 : 4)) == REG_TO_IMMD_SUBMASK(modrm_reg(modrm)))
				? SUBMASK_TO_LEN(sub)
				: 0;
		}
		return len;
	}

	// Calculate 'pointing' opcode byte (The one that indicates the opcode)
	u8 shift = (opcode.len - 1) << 3;
	u8 opi = (opcode.val >> shift) & 0xff;

	// Access correct immediate mask based on 3 trailing bits
	res = INTEL64_IMMD_VTB(L2I_MASK_LOOKUP[op0 & 0x7], opi);

	// If submask is present calculate sub value with modrm
	if (res == 0b11
	&& opi >= 0x3f){ // Submask present
		sub = (L2I_SUBMASK_LOOKUP[op0 & 0x7])[opi - 0x7f];
		len = ((sub >> (sub & 1 ? 1 : 4)) == REG_TO_IMMD_SUBMASK(modrm_reg(modrm)))
			? SUBMASK_TO_LEN(sub)
			: 0;
	}else{
		len = MASK_TO_LEN(res);
	}
	return len; 
}

/*
 	Opcode checking immidiate read.
*/
_inline_force u64 _intel64_get_immd(
	_in intel64_opcode		opcode,
	_in u8				modrm,
	_in intel64_mte_raw_instr	instr
){
	u8 len = _intel64_immd_len(opcode, modrm);
	u8 immd_i = opcode.info.r 
			+ (opcode.info.l & ~opcode.info.e)
			+ opcode.len
			+ _intel64_sib_ext(modrm)
			+ _intel64_disp_len(opcode, modrm);
	immd_i += !(modrm == 0);
	io_i64(opcode.info.l);
	io_i64(opcode.info.r);

	u8 buf[8] = {0}; // 64 bit buffer
	for (u8 i = 0; i < len; i++){
		buf[i] = instr[immd_i + i];
	}
	return *(u64*)buf;
}

#endif // !defined(AX_INTEL64_INSTR_INT)

