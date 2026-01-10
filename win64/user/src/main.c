#include "mte/core.h"
#include "mte/ir.h"
#include "mte/asm/mips/mips32_asm.h"
#include "mte/asm/mips/mips32_ir.h"
#include "mte/asm/x86/x86_64.h"

_inline_avert void foo(
	x86_64_mte_raw_instr instr){
	u64 l1, l2;

	(void)__rdtsc();
	l1 = __rdtsc();
	u32 res = 0;
	res = _x86_64_opcode(instr);
	l2 = __rdtsc();
	printf("Time in ns: %lf\n", ((l2 - l1) / 4.2) - 4);
	io_i64(res);
}
int main(){
	x86_64_mte_raw_instr instr = init_x86_64_mte_raw_instr(0x66, 0x0F, 0x3A, 0x0F, 0xC1, 0x08);
	foo(instr);
	//io_i64(res);
	/*ir_context *mips32_ir = nullptr;
	ir_create(mips32, &mips32_ir);*/

	//ir_raw_instr *instr = nullptr;
	/*const char str[] = "       add $t1,$t2,$t3";
	mte_raw_instr enc = {0};
	const ir_rule *rule = nullptr;
	mips32_byte_to_raw(
		&(mte_byte_instr){
			.syn = intel,
			.arch = mips32,
			.val = _str_to_u64(str, strlen(str))
		},
		&rule,
		&enc
	);
	mips32_byte_to_raw(
		&(mte_byte_instr){
			.syn = intel,
			.arch = mips32,
			.val = _str_to_u64(str, strlen(str))
		},
		&rule,
		&enc
	);

	ir_raw_instr instr = init_ir_raw_instr(nullptr);
	mips32_raw_to_ir(enc.mips32, rule, &instr);*/


	/*for (int i = 0; i < 100; i++){
		mte_u64_instr v = _str_to_u64(str, strlen(str));
	mips32_byte_to_raw(
		&(mte_byte_instr){
			.syn = intel,
			.arch = mips32,
			.val = v
		},
		&enc
	);
		axfree(v.org);
	}*/
	//io_i64(mips32_funct(enc.mips32));
	//io_i64(_u64_qlen(0x646461));
	//printf("%s\n", (mips32_op_lookup(0xffULL).mnem));

	/*ir_raw_instr buf = init_ir_raw_instr(0);
	mips32_raw_to_ir(enc.mips32, &buf);*/

	return 0;
}

