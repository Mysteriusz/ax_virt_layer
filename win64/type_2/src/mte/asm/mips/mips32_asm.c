#include <ax_memory.h>
#include "mte/asm/mips/mips32_asm.h"

static void _eval_type_r(
	_in mte_byte_instr		*instr,
	_out mips32_mte_raw_instr	*buf
){
	if (mte_byte_instr_inv(instr)){
		return;
	}

	const c8 *restrict lb_char = _unsafe_skip_until_not(instr->buf, '\x20'); // left_bound
	const c8 *restrict rb_char = _unsafe_skip_until(lb_char, '\x20'); // right_bound
	u8 i = 0;
	while(*lb_char != '\0'){
		// Evaluate range
		*((u32*)buf) |= 
			((mips32_eval_r(lb_char, rb_char, i) & _mips32_mask_r[i])
			<< _mips32_shift_r[i]);

		lb_char = _unsafe_skip_until_not(rb_char, ',');
		lb_char = _unsafe_skip_until_not(lb_char, '\x20');
		rb_char = _unsafe_skip_until(lb_char, ',');
		i++;
	}
}

axres mips32_byte_to_raw(
	_in mte_byte_instr			*instr,
	_out mte_raw_instr			*buf
){
	if (instr == nullptr){
		return AX_INV_ARG;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	_eval_type_r(instr, &buf->mips32);
	if (mips32_opcode(buf->mips32) == MIPS32_OPCODE_INVALID){
		return AX_MTE_INV_INSTR;
	}
	buf->arch = instr->arch;

	return AX_SUCC;
}


u8 mips32_eval_r(
	_in register const c8	*restrict 	lb_char,
	_in register const c8	*restrict 	rb_char,
	_in u8 					i
){
	// Evaluate based on operation index
	switch(i){
	case 0:
		return mips32_opcode_lookup(lb_char, rb_char - lb_char);
	case 1:
	case 2:
	case 3:
		return mips32_reg_lookup(lb_char, rb_char - lb_char);
	default:
		return 0;
	}
}

