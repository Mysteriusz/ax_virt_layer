#if !defined(MTE_I64_IMMD_INT)
#define MTE_I64_IMMD_INT

#include <ax_type.h>

/*
 	For MTE_I64_QTABLES_INT
*/

/*
 	Immediate table definitions
*/
#define SUBMASK_IMMD_8 		0b0001
#define SUBMASK_IMMD_16 	0b0010
#define SUBMASK_IMMD_32 	0b0100	
#define SUBMASK_IMMD_64 	0b0110

#define SUBMASK_IMMD_MOD_0 	0b00000000
#define SUBMASK_IMMD_MOD_1 	0b00000010
#define SUBMASK_IMMD_MOD_2 	0b00000100
#define SUBMASK_IMMD_MOD_3 	0b00001000
#define SUBMASK_IMMD_MOD_4 	0b00010000
// Extended
#define SUBMASK_IMMD_MOD_5 	0b00100000
#define SUBMASK_IMMD_MOD_6 	0b01000000
#define SUBMASK_IMMD_MOD_7 	0b10000000

// Universal submask creator
#define SUBMASK_IMMD_AUTO(mask, immd) ( \
	(mask) << ((immd == SUBMASK_IMMD_8) ? 0 : 3) | immd)

// modrm.reg to immediate submask (SUBMASK_IMMD_MOD_N)
#define REG_TO_IMMD_SUBMASK(r) (((r) == 0) ? 0 : (1 << (r)))


/*
	MASK:
 	If ln_??_mask[i] == 00 then:
		NO IMMEDIATE
 	If ln_??_mask[i] == 01 then:
		len = 1 byte
 	If ln_??_mask[i] == 10 then:
		len = 4 bytes
 	If ln_??_mask[i] == 11 then:
		IMMEDIATE SUBMASK PRESENT

	SUBMASK:
 	If (ln_??_submask[i] & 1) == 1 then:
		len = 1 byte (SUBMASK_IMMD_8)
 	If ((ln_??_submask[i] >> 1) & 3) == 01 then:
		len = 2 bytes (SUBMASK_IMMD_16)
 	If ((ln_??_submask[i] >> 1) & 3) == 10 then:
		len = 4 bytes (SUBMASK_IMMD_32)
 	If ((ln_??_submask[i] >> 1) & 3) == 11 then:
		len = 8 bytes (SUBMASK_IMMD_64)
*/
struct immd_tables_root{
	// 64 + 128 bytes
	const u64 	l0_mask[8];
	const u8 	l0_submask[128]; // 0x00 - 0x7f opcodes don`t use the submask
	// 64 + 192 bytes
	const u64 	l2_66_mask[8];
	const u8 	l2_66_submask[192]; // 0x00 - 0x3f opcodes don`t use the submask
	// 64 + 192 bytes
	const u64 	l2_0f_mask[8];
	const u8 	l2_0f_submask[192]; // 0x00 - 0x3f opcodes don`t user the submask
};
extern const struct immd_tables_root IMMD_TABLES _align(64);
#define I64_IMMD_VTB(m, v) ((m[(v) >> 5] >> (((v) * 2) & 63)) & 0x3)

/*
 	Prefetch immd tables
*/

_inline_avert void i64_prefetch_immd(
	void
);

#endif // !defined(MTE_I64_IMMD_INT)

