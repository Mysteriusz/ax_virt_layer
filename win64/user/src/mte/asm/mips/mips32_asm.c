#include <ax_memory.h>
#include "mte/asm/mips/mips32_asm.h"
#include "mte/asm/bits.h"

axres mips32_byte_to_raw(
	_in register mte_byte_instr		*instr,
	_out const ir_rule			**rule,
	_out register mte_raw_instr		*buf
){
	if (instr == nullptr){
		return AX_INV_ARG;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	_mips32_eval(instr, rule, &buf->mips32);
	if (mips32_opcode(buf->mips32) == MIPS32_OPCODE_INVALID){
		return AX_MTE_INV_INSTR;
	}
	buf->arch = instr->arch;

	return AX_SUCC;
}

mips32_mte_raw_instr mips32_eval_type_r(
	_in register mte_u64_instr *const instr 
){
	asrt(instr != nullptr);

	register mips32_mte_raw_instr buf = 0;

	/*
	 	1st register (rd)
	*/

	register u64 *lhs = nullptr;
	register u64 *rhs = nullptr;

	_u64_byte_skip(0x20, instr); // Skip to left side of the register
	lhs = instr->ptr;

	_u64_byte_search(0x2c, instr); // Skip to the right side of the register
	rhs = instr->ptr;

	load_bits(
		buf,
		// Lookup only the non spaced range
		_mips32_reg_lookup(
			*lhs & n_mask(_u64_real_dist(lhs, rhs))
		).val,
		MIPS32_RD_SHIFT);

	_u64_move(1, instr);

	/*
	 	2nd register (rs)
	*/

	_u64_byte_skip(0x20, instr); // Skip to left side of the register
	lhs = instr->ptr;

	_u64_byte_search(0x2c, instr); // Skip to the right side of the register
	rhs = instr->ptr;

	load_bits(
		buf,
		// Lookup only the non spaced range
		_mips32_reg_lookup(
			*lhs & n_mask(_u64_real_dist(lhs, rhs))
		).val,
		MIPS32_RS_SHIFT);

	/*
	 	3rd register (rt)
	*/

	_u64_move(1, instr);

	_u64_byte_skip(0x20, instr); // Skip to left side of the register
	lhs = instr->ptr;

	_u64_byte_search(0x00, instr);
	rhs = instr->ptr;

	load_bits(
		buf,
		// Lookup only the non spaced range
		_mips32_reg_lookup(
			*lhs & n_mask(_u64_real_dist(lhs, rhs))
		).val,
		MIPS32_RT_SHIFT);

	return buf;
}

mips32_mte_raw_instr mips32_eval_type_i(
	_in register mte_u64_instr *const instr 
){
	asrt(instr != nullptr);
	return (mips32_mte_raw_instr){0};
}

mips32_mte_raw_instr mips32_eval_type_j(
	_in register mte_u64_instr *const instr 
){
	asrt(instr != nullptr);
	return (mips32_mte_raw_instr){0};
}

