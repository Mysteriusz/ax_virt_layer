#if !defined(MTE_INTEL64_MODRM_INT)
#define MTE_INTEL64_MODRM_INT

#include <ax_type.h>

/*
 	Each table consists of ranges at specific byte index in the opcode,
	which indicates if opcode has modrm byte 

	Reference source: http://ref.x86asm.net/coder64.html
*/

struct modrm_tables_root{
	// 2 * 32 bytes
	const u64 l0_mask[4];
	const u64 l1_mask[4];
	// 2 * 32 bytes
	const u64 l2_0f_mask[4];
	const u64 l2_66_mask[4];
	// 2 * 32 bytes
	const u64 l2_f2_mask[4];
	const u64 l2_f3_mask[4];
};
extern const struct modrm_tables_root *const modrm_tables _align(64);
#define INTEL64_MODRM_VTB(m, v) ((m[(v) >> 6] >> ((v) & 63)) & 1)

/*
 	Prefetch modrm tables
*/

static void _intel64_prefetch_modrm(
	void
){
	_mm_prefetch(modrm_tables, _MM_HINT_T0);
	_mm_prefetch(offp(modrm_tables, 64), _MM_HINT_T0);
	_mm_prefetch(offp(modrm_tables, 128), _MM_HINT_T0);
};

#endif // !defined(MTE_INTEL64_MODRM_INT)

