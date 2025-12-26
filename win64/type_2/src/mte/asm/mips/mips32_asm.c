#include <ax_memory.h>
#include "mte/asm/mips/mips32_asm.h"
#include "mte/asm/decode_u64.h"

axres mips32_byte_to_raw(
	_in register mte_byte_instr		*instr,
	_out register mte_raw_instr		*buf
){
	if (instr == nullptr){
		return AX_INV_ARG;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	_mips32_eval(instr, &buf->mips32);
	if (mips32_opcode(buf->mips32) == MIPS32_OPCODE_INVALID){
		return AX_MTE_INV_INSTR;
	}
	buf->arch = instr->arch;

	return AX_SUCC;
}

axres mips32_eval_type_r(
	_in register mte_u64_instr *const instr 
){
	asrt(instr != nullptr);

	/*
	 	1st register (rd)
	*/

	register u64 *lhs = nullptr;
	register u64 *rhs = nullptr;

	_u64_byte_skip(0x20, instr); // Skip to left side of the register
	lhs = instr->ptr;
	_u64_byte_search(0x2c, instr); // Skip to the right side of the register
	rhs = instr->ptr;

	const struct mips32_reg_info r1 = 
		_mips32_reg_lookup(*lhs & n_mask((u64)rhs - (u64)lhs));

	instr->ptr = (u64*)(((u64)instr->ptr) + 1);

	/*
	 	2nd register (rt)
	*/

	_u64_byte_skip(0x20, instr); // Skip to left side of the register
	lhs = instr->ptr;
	_u64_byte_search(0x2c, instr); // Skip to the right side of the register
	rhs = instr->ptr;

	const struct mips32_reg_info r2 = 
		_mips32_reg_lookup(*lhs & n_mask((u64)rhs - (u64)lhs));

	/*
	 	3rd register (rs)
	*/

	instr->ptr = (u64*)(((u64)instr->ptr) + 1);

	_u64_byte_skip(0x20, instr); // Skip to left side of the register
	lhs = instr->ptr;
	_u64_byte_search(0x2c, instr); // Skip to the right side of the register
	rhs = instr->ptr;

	const struct mips32_reg_info r3 = 
		_mips32_reg_lookup(*lhs & n_mask((u64)rhs - (u64)lhs));

	return AX_SUCC;
}

