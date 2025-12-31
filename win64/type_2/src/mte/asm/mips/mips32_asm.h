/*
 	MIPS32 assembler
*/

#include "mte/core.h"
#include "mte/ir.h"

#include "mte/asm/mips/mips32.h"
#include "mte/asm/mips/mips32_op_lookup.h"
#include "mte/asm/mips/mips32_reg_lookup.h"
#include "mte/asm/bits.h"

/*
 	Currently achives speeds around
		- ~200ns for 4.2GHZ cpu
*/
axres mips32_byte_to_raw(
	_in mte_byte_instr			*instr,
	_out mte_raw_instr			*buf
);

/*
	R-type instruction has 4 steps
	Shifts are based on struct mte_raw_instr_mips32
	By default all operations are performed and stored as LITTLE-ENDIAN

	1) funct (Ex: add, addi, xor) shift = 0
	TODO: 2) shamt (Ex:) shift = 6
	2) rd (Ex: $t2, $t7) shift = 11
	3) rs (Ex: $t1, $t3) shift = 21
	4) rt (Ex: $t0, $t3) shift = 16
*/

static u32 _mips32_shift_r[4] = {0, 11, 21, 16}; 
static u32 _mips32_mask_r[4] = {0x3f, 0x1f, 0x1f, 0x1f}; 
mips32_mte_raw_instr mips32_eval_type_r(
	_in register mte_u64_instr *const instr 
);
mips32_mte_raw_instr mips32_eval_type_i(
);
mips32_mte_raw_instr mips32_eval_type_j(
);

_inline_force static void _mips32_eval(
	_in register mte_byte_instr		*instr,
	_out register mips32_mte_raw_instr	*buf
){
	if (mte_byte_instr_inv(instr)){
		return;
	}
	u64 l1, l2;

	__builtin_prefetch(buf);
	(void)__rdtsc();
	l1 = __rdtsc();

	register u64 *lhs = nullptr;
	register u64 *rhs = nullptr;

	register mte_u64_instr *const instr_ptr = &instr->val;

	_u64_byte_skip(0x20, instr_ptr);
	lhs = instr_ptr->ptr;
	_u64_byte_search(0x20, instr_ptr);
	rhs = instr_ptr->ptr;

	const struct mips32_op_info info =
		_mips32_op_lookup(*lhs & n_mask((u64)rhs - (u64)lhs));

	// Skip to first register
	switch(info.type){
	case R:
		load_bits(*buf, info.opcode, MIPS32_SHAMT_SHIFT);
		*buf |= mips32_eval_type_r(instr_ptr);
		break;
	case I:
		//mips32_eval_type_i();
		break;
	case J:
		//mips32_eval_type_j();
		break;
	default:
		return;
	}

	l2 = __rdtsc();
	printf("Time in ns: %lf\n", ((l2 - l1) / 4.2) - 4);
	printf("Mnemonic: %s\n", (char*)&info.mnem_u64);
	printf("%u\n", info.type);
	printf("Encoded: %u\n", *buf);
	printf("%s\n", (char*)lhs);
}
