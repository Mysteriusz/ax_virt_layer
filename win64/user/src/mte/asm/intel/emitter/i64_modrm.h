#if !defined(MTE_I64_MODRM_INT)
#define MTE_I64_MODRM_INT

#include <ax_type.h>

#include "i64_emit_info.h"
#include "i64_rex.h"

static u8 _i64_modrm_resolve(
	_in enum i64_operand_id		dest,
	_in enum i64_operand_id		src,
	_in struct i64_operand_sum	sum
){
	/*
	 	0000DCBA
		D -> 32-bit displacement
		C -> 8-bit displacement
		B -> SIB addressing
		A -> Memory addressing
	*/
	u8 mod_i = !!(sum.width & BIT(6)) << 3 |
		!!(sum.width & BIT(5)) << 2 |
		!!(sum.operand & BIT(4)) << 1 |
		!!(sum.operand & BIT(6));

	bool flip = !!(sum.operand & BIT(7));

	u8 dest_flip = (flip ? dest : src);
	u8 src_flip = (flip ? src : dest);

	/*
	 	Groups 32 bits into 2-bit mod encoding
		[15] -> 0b00
		[14] -> 0b00
		[13] -> 0b00
		[12] -> 0b00
		[0b1011] -> 0b10  // 32-bit displacement
		[10] -> 0b00
		[0b1001] -> 0b10  // 32-bit displacement
		[08] -> 0b00
		[0b0111] -> 0b01  // 8-bit displacement
		[06] -> 0b00
		[0b0101] -> 0b01  // 8-bit displacement
		[04] -> 0b00
		[03] -> 0b00
		[02] -> 0b00
		[01] -> 0b00 
    		[0b0000] -> 0b11  // Register to register

	*/
	u32 mod_magic = 0x00884403; 
	u8 calc = (mod_magic >> (mod_i << 1)) & 0b11;
	u8 rm = 0;

	if (!!(mod_i & BIT(1))){ // B
		rm = 0b100;
	}else{
		rm = dest_flip & 0b111;
	}

	u8 modrm = calc << 6 | (src_flip & 0b111) << 3 | rm;

	return modrm;
}

/*
 	For MTE_I64_QTABLES_INT
*/

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

