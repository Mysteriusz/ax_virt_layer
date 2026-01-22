/*
 	MIPS32 assembler
*/

#include "mte/core.h"
#include "mte/ir/ir.h"

#include "mips32.h"
#include "tables/mips32_op_lookup.h"
#include "tables/mips32_reg_lookup.h"
#include "mte/asm/bits.h"

#include "mte/perf.h"

/*
 	Currently achives speeds around
		- ~1000ns for 4.2GHZ CPU with cold cache
		- ~25-300ns for 4.2GHZ CPU with warmed cache
*/
axres mips32_byte_to_raw(
	_in mte_byte_instr			*instr,
	_out const ir_rule			**rule,
	_out mte_raw_instr			*buf
);

/*
	R-type instruction encoding has 3/4 steps
	Shifts are based on struct mte_raw_instr_mips32
	By default all instructions are expected to be LITTLE-ENDIAN

	1) funct (Ex: add, addi, xor) shift = 0
	TODO SUPPORT: 2) shamt (Ex:1, 6, 9) shift = 6
	2) rd (Ex: $t2, $t7) shift = 11
	3) rs (Ex: $t1, $t3) shift = 21
	4) rt (Ex: $t0, $t3) shift = 16

	Example insturction without shamt field:
		add $t0 -> (rd), $t1 -> (rs), $t2 -> (rt)
	With shamt:
		sll $t0 -> (rd), $t1 -> (rt), 1 -> (shamt)
*/
mips32_mte_raw_instr mips32_eval_type_r(
	_in register mte_u64_instr *const instr 
);

/*
	I-type instruction encoding has 4 steps
*/
mips32_mte_raw_instr mips32_eval_type_i(
	_in register mte_u64_instr *const instr 
);

/*
	J-type instruction encoding has 3 steps
*/
mips32_mte_raw_instr mips32_eval_type_j(
	_in register mte_u64_instr *const instr 
);

_inline_force static void _mips32_eval(
	_in register mte_byte_instr		*instr,
	_out register const ir_rule		**rule,
	_out register mips32_mte_raw_instr	*buf
){
	if (mte_byte_instr_inv(instr)){
		return;
	}

	//__INL_PERF_INIT
	//__INL_PERF_START

	register u64 *lhs = nullptr;
	register u64 *rhs = nullptr;

	register mte_u64_instr *const instr_ptr = &instr->val;

	_u64_byte_skip(0x20, instr_ptr);
	lhs = instr_ptr->ptr;
	_u64_byte_search(0x20, instr_ptr);
	rhs = instr_ptr->ptr;

	__builtin_prefetch(mips32_eval_type_r);
	__builtin_prefetch(mips32_eval_type_j);
	__builtin_prefetch(mips32_eval_type_i);

	const struct mips32_op_info info =
		_mips32_op_lookup(*lhs & n_mask((u64)rhs - (u64)lhs));

	// Skip to first register
	switch(info.type){
	case R:
		load_bits(*buf, info.opcode, MIPS32_SHAMT_SHIFT);
		*rule = &info.ir_rule;
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

	//__INL_PERF_END
	//printf("Time in ns: %lf\n", (__INL_PERF_SUM / 4.2));
	/*printf("Time in ns: %lf\n", ((l2 - l1) / 4.2) - 4);
	printf("Mnemonic: %s\n", (char*)&info.mnem_u64);
	printf("%u\n", info.type);
	printf("Encoded: %u\n", *buf);
	printf("%s\n", (char*)lhs);*/
}
