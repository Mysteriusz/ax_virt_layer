#if !defined(MTE_I64_MODRM_INT)
#define MTE_I64_MODRM_INT

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
extern const struct modrm_tables_root MODRM_TABLES _align(64);

// Access modrm table (t) to opcode leading byte (v)
// The modrm table may be any of the tables from [MODRM_TABLES]
#define I64_MODRM_VTB(t, v) \
	((t[(v) >> 6 /* Divide by 64 */] >> ((v) & 63)) & 1 /* Shift to index and read present bit */)

/*
 	Prefetch modrm tables
*/

_inline_avert void i64_prefetch_modrm(
	void
);

#endif // !defined(MTE_I64_MODRM_INT)

