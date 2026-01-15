#if !defined(X86_64_MODRM_INT)
#define X86_64_MODRM_INT

#include <ax_type.h>
#include <stdint.h>
#include <ext_simd.h>

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
extern const struct modrm_tables_root modrm_tables _align(64);
/*static const bool table_l0[256] = {
	[0x00 ... 0x03] = true,
	[0x08 ... 0x0B] = true,
	[0x10 ... 0x13] = true,
	[0x18 ... 0x1B] = true,
	[0x20 ... 0x23] = true,
	[0x28 ... 0x2B] = true,
	[0x30 ... 0x33] = true,
	[0x38 ... 0x3B] = true,
	[0x62 ... 0x63] = true,
	[0x69] = true,
	[0x6B] = true,
	[0x80 ... 0x8f] = true,
	[0xC0 ... 0xC1] = true,
	[0xC6 ... 0xC7] = true,
	[0xD0 ... 0xD3] = true,
	[0xD8 ... 0xDB] = true,
	[0xDC ... 0xDF] = true,
	[0xF6 ... 0xF7] = true,
	[0xFE ... 0xFF] = true
};
static const bool table_l1[256] = {
	[0x00 ... 0x03] = true,
	[0x10 ... 0x18] = true,
	[0x1f ... 0x2f] = true,
	[0x40 ... 0x6b] = true,
	[0x6e ... 0x76] = true,
	[0x78 ... 0x79] = true,
	[0x7e ... 0x7f] = true,
	[0x90 ... 0x9f] = true,
	[0xa3 ... 0xa5] = true,
	[0xab ... 0xc7] = true,
	[0xd1 ... 0xd5] = true,
	[0xd7 ... 0xe5] = true,
	[0xe7 ... 0xef] = true,
	[0xf1 ... 0xfe] = true,
};
static const bool table_l2_0f[256] = {
	[0xc1 ... 0xc4] = true,
	[0xc8 ... 0xc9] = true,
	[0xd0 ... 0xd1] = true,
	[0xf8 ... 0xf9] = true,
	[0xf0 ... 0xf1] = true,
	[0x0f] = true,
};
static const bool table_l2_66[256] = {
	[0x10 ... 0x17] = true,
	[0x28 ... 0x2f] = true,
	[0x50 ... 0x51] = true,
	[0x54 ... 0x76] = true,
	[0x7c ... 0x7f] = true,
	[0xc2] = true,
	[0xc4 ... 0xc7] = true,
	[0xd0 ... 0xef] = true,
	[0xf1 ... 0xfe] = true,
};
static const bool table_l2_f2[256] = {
	[0x10 ... 0x12] = true,
	[0x2a] = true,
	[0x2c ... 0x2d] = true,
	[0x51] = true,
	[0x58 ... 0x5a] = true,
	[0x5c ... 0x5f] = true,
	[0x70] = true,
	[0x7c ... 0x7f] = true,
	[0xc2] = true,
	[0xd0] = true,
	[0xd6] = true,
	[0xe6] = true,
	[0xf0] = true,
};
static const bool table_l2_f3[256] = {
	[0x10 ... 0x12] = true,
	[0x16] = true,
	[0x2a] = true,
	[0x2c ... 0x2d] = true,
	[0x51 ... 0x53] = true,
	[0x58 ... 0x5f] = true,
	[0x6f ... 0x70] = true,
	[0x7e ... 0x7f] = true,
	[0xb8] = true,
	[0xc2] = true,
	[0xc7] = true,
	[0xd6] = true,
	[0xe6] = true,
};*/
#define VAL_TO_BIT(m, v) ((m[(v) >> 6] >> ((v) & 63)) & 1)

/*
 	Prefetch modrm tables
*/

static void _x86_64_prefetch_modrm(void){
	_mm_prefetch(&modrm_tables, _MM_HINT_T0);
	_mm_prefetch(offp(&modrm_tables, 64), _MM_HINT_T0);
	_mm_prefetch(offp(&modrm_tables, 128), _MM_HINT_T0);
};

#endif // !defined(X86_64_MODRM_INT)

