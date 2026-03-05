#if !defined(MTE_I64_SIB_INT)
#define MTE_I64_SIB_INT

#include <ax_type.h>
#include <ax_io.h>

#include "i64_emit_info.h"

static u8 _i64_sib_resolve(
	_in enum i64_operand_id		base_id,
	_in struct i64_operand_mem 	mem,
	_in struct i64_operand_sum	sum
){
	// SIB not present
	if (!!!(sum.operand & BIT(4))){ // E
		return 0;
	}

	/*
	 	SIB byte structure follows:

		S -> Scale bits
		I -> Index extension bits
		B -> Base extension bits

		SSIIIBBB
	*/
	u8 sib = 0;

	/*
	 	Resolve [sib.index] AND [sib.base]
	*/
	u8 sib_index = (mem.index_id & 0b111) << 3;
	u8 sib_base = base_id & 0b111;

	sib = sib_index | sib_base;

	/*
	 	Resolve [sib.scale]

	 	Groups 8 bits into 2-bit [sib.scale] encoding

	   	Index of each group is [mem.scale_id] & 0b11
		[03] -> 0b11  // * 8 scale
		[02] -> 0b10  // * 4 scale
    		[01] -> 0b01  // * 2 scale
    		[00] -> 0b00  // * 1 scale

	*/
	const u8 sib_magic = 0xe4;

	// Calculate scale and shift to left
	u8 sib_scale =
		(sib_magic >> ((mem.scale_id & 0b11) << 1)) << 6;

	sib |= sib_scale;

	return sib;
}

#endif // !defined(MTE_I64_SIB_INT)

