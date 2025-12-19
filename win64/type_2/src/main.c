#include <pthread.h>
#include "mte/ir/core.h"

int main(){
	ir_context *mips32_ir = nullptr;
	ir_create(mips32, &mips32_ir);

	ir_raw_instr *instr = nullptr;
	c8 raw[] = "add rax, rbx";

	mips32_ir->call.queue(
		mips32_ir,
		&(mte_byte_instr){
			.arch = mips32,
			.buf = raw,
			.len = sizeof(raw)
		},
		&instr);

	ir_close(mips32_ir);
	return 0;
}

