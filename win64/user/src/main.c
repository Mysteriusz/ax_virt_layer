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
	x86_64_mte_raw_instr instr1 = init_x86_64_mte_raw_instr(0x66, 0x45, 0x0f, 0x38, 0x00);
	foo(instr1);
	x86_64_mte_raw_instr instr2 = init_x86_64_mte_raw_instr(0x41, 0x45);
	foo(instr2);
	x86_64_mte_raw_instr instr3 = init_x86_64_mte_raw_instr(0x4c, 0x89, 0xf8);
	foo(instr3);
	x86_64_mte_raw_instr instr4 = init_x86_64_mte_raw_instr(0x66, 0x41, 0x0F, 0x38, 0x00, 0x0A);
	foo(instr4);
	x86_64_mte_raw_instr instr5 = init_x86_64_mte_raw_instr(0x0F, 0x01, 0xC1);
	foo(instr5);
	x86_64_mte_raw_instr instr6 = init_x86_64_mte_raw_instr(0x66, 0x0f, 0x38, 0xc1);
	foo(instr6);
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

