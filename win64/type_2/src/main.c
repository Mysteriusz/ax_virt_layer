#include <pthread.h>
#include "mte/ir/core.h"
#include "mte/arch/mips/mips32_conv.h"

int main(){
	ir_context *mips32_ir = nullptr;
	ir_create(mips32, &mips32_ir);

	//ir_raw_instr *instr = nullptr;
	c8 raw[] = "add $t0,$t1,$t2";

	struct mips32_mte_raw_instr buf = {0};
	mips32_byte_to_raw(
		&(mte_byte_instr){
			.syn = intel,
			.arch = mips32,
			.buf = raw,
			.len = sizeof(raw),
		},
		&buf
	);

	/*mips32_ir->call.queue(
		mips32_ir,
		&(mte_byte_instr){
			.arch = mips32,
			.buf = raw,
			.len = sizeof(raw),
			.syn = intel
		},
		&instr);*/

	ir_close(mips32_ir);
	return 0;
}

