#include "mte/core.h"
#include "mte/ir.h"
#include "mte/asm/mips/mips32_asm.h"
#include "mte/asm/mips/mips32_ir.h"

int main(){
	/*ir_context *mips32_ir = nullptr;
	ir_create(mips32, &mips32_ir);*/

	//ir_raw_instr *instr = nullptr;

	const char str[] = "       add $t1,$t2,$t3";
	mte_raw_instr enc = {0};
	mips32_byte_to_raw(
		&(mte_byte_instr){
			.syn = intel,
			.arch = mips32,
			.val = _str_to_u64(str, strlen(str))
		},
		&enc
	);
	mips32_byte_to_raw(
		&(mte_byte_instr){
			.syn = intel,
			.arch = mips32,
			.val = _str_to_u64(str, strlen(str))
		},
		&enc
	);
	//io_i64(mips32_funct(enc.mips32));
	//io_i64(_u64_qlen(0x646461));
	//printf("%s\n", (mips32_op_lookup(0xffULL).mnem));

	/*ir_raw_instr buf = init_ir_raw_instr(0);
	mips32_raw_to_ir(enc.mips32, &buf);*/

	return 0;
}

