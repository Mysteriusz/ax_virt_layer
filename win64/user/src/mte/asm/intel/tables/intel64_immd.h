#if !defined(INTEL64_IMMD_INT)
#define INTEL64_IMMD_INT

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
	// 64 + 192 bytes
	const u64 	l2_66_mask[8];
	const u8 	l2_66_submask[192]; // 0x00 - 0x3f opcodes don`t use the submask
	// 64 + 192 bytes
	const u64 	l2_0f_mask[8];
	const u8 	l2_0f_submask[192]; // 0x00 - 0x3f opcodes don`t user the submask
};
extern const struct immd_tables_root *const immd_tables _align(64);
#define INTEL64_IMMD_VTB(m, v) ((m[(v) >> 5] >> (((v) * 2) & 63)) & 0x3)

/*
 	Prefetch immd tables
*/

static void _intel64_prefetch_immd(void){
	// L0 
	_mm_prefetch(immd_tables, _MM_HINT_T0);
	_mm_prefetch(offp(immd_tables, 64), _MM_HINT_T0);
	_mm_prefetch(offp(immd_tables, 64 * 2), _MM_HINT_T0);
	// L2_66
	_mm_prefetch(offp(immd_tables, 64 * 3), _MM_HINT_T0);
	_mm_prefetch(offp(immd_tables, 64 * 4), _MM_HINT_T0);
	_mm_prefetch(offp(immd_tables, 64 * 5), _MM_HINT_T0);
	_mm_prefetch(offp(immd_tables, 64 * 6), _MM_HINT_T0);
	// L2_0f
	_mm_prefetch(offp(immd_tables, 64 * 7), _MM_HINT_T0);
	_mm_prefetch(offp(immd_tables, 64 * 8), _MM_HINT_T0);
	_mm_prefetch(offp(immd_tables, 64 * 9), _MM_HINT_T0);
	_mm_prefetch(offp(immd_tables, 64 * 10), _MM_HINT_T0);
};

#endif // !defined(INTEL64_IMMD_INT)

