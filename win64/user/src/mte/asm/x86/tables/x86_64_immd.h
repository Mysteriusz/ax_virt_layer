#if !defined(X86_64_IMMD_INT)
#define X86_64_IMMD_INT

#include <ax_type.h>

#define SUBMASK_IMMD_8 		0b0001
#define SUBMASK_IMMD_16 	0b0010
#define SUBMASK_IMMD_32 	0b0100	
#define SUBMASK_IMMD_64 	0b0110

/*
 	If (submask & 0x7) == SUBMASK_IMMD_8 then:
		SUBMASK_IMMD_MOD_N is extended to 7 bits
*/

#define SUBMASK_IMMD_MOD_0 	0b00000000
#define SUBMASK_IMMD_MOD_1 	0b00000010
#define SUBMASK_IMMD_MOD_2 	0b00000100
#define SUBMASK_IMMD_MOD_3 	0b00001000
#define SUBMASK_IMMD_MOD_4 	0b00010000
// Extended
#define SUBMASK_IMMD_MOD_5 	0b00100000
#define SUBMASK_IMMD_MOD_6 	0b01000000
#define SUBMASK_IMMD_MOD_7 	0b10000000

#define REG_TO_IMMD_SUBMASK(r) (((r) == 0) ? 0 : (1 << (r)))

struct immd_tables_root{
	// 64 + 128 bytes
	const u64 	l0_mask[8];
	const u8 	l0_submask[128]; // 0x00 - 0x7f opcodes don`t use the submask
	// 64 + 256 bytes
	const u64 	l2_66_mask[8];
	const u8 	l2_66_submask[256];
};
extern const struct immd_tables_root immd_tables _align(64);
#define X86_64_IMMD_VTB(m, v) ((m[(v) >> 5] >> (((v) * 2) & 63)) & 0x3)

/*
 	Prefetch immd tables
*/

static void _x86_64_prefetch_immd(void){
	_mm_prefetch(&immd_tables, _MM_HINT_T0);
	_mm_prefetch(offp(&immd_tables, 64), _MM_HINT_T0);
	_mm_prefetch(offp(&immd_tables, 128), _MM_HINT_T0);
	_mm_prefetch(offp(&immd_tables, 192), _MM_HINT_T0);
};

#endif // !defined(X86_64_IMMD_INT)

