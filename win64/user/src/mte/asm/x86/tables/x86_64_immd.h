#if !defined(X86_64_IMMD_INT)
#define X86_64_IMMD_INT

#include <ax_type.h>

#define SUBMASK_IMMD_8 		0x01
#define SUBMASK_IMMD_16 	0x02	
#define SUBMASK_IMMD_32 	0x04	
#define SUBMASK_IMMD_64 	0x08
#define SUBMASK_IMMD_MOD_0 	0x10
#define SUBMASK_IMMD_MOD_1 	0x20
#define SUBMASK_IMMD_MOD_2 	0x40
#define SUBMASK_IMMD_MOD_3 	0x80
#define REG_TO_IMMD_SUBMASK(r) ((1 << (r)))

struct immd_tables_root{
	// 64 + 128 bytes
	const u64 l0_mask[8];
	const u8 l0_submask[128]; // 0x00 - 0x7f dont use the submask
	// 2 * 64 bytes
	const u64 l2_0f_mask[8];
	const u64 l2_66_mask[8];
	// 2 * 64 bytes
	const u64 l2_f2_mask[8];
	const u64 l2_f3_mask[8];
};
extern const struct immd_tables_root immd_tables _align(64);
#define X86_64_IMMD_VTB(m, v) ((m[(v) >> 5] >> (((v) * 2) & 63)) & 0x3)
#define X86_64_IMMD_SVTB(m, v) (m[(v) - 0xb0])

/*
 	Prefetch immd tables
*/

static void _x86_64_prefetch_immd(void){
	_mm_prefetch(&immd_tables, _MM_HINT_T0);
	_mm_prefetch(offp(&immd_tables, 64), _MM_HINT_T0);
	_mm_prefetch(offp(&immd_tables, 128), _MM_HINT_T0);
};

#endif // !defined(X86_64_IMMD_INT)

