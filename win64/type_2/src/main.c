#include "mte/core.h"
#include "mte/ir.h"
#include "mte/asm/mips/mips32_asm.h"
#include "mte/asm/mips/mips32_ir.h"

int main(){
	/*ir_context *mips32_ir = nullptr;
	ir_create(mips32, &mips32_ir);*/

	//ir_raw_instr *instr = nullptr;
	c8 raw[] = "addi $t0,$t1,$t2";

	mte_raw_instr enc = {0};
	mips32_byte_to_raw(
		&(mte_byte_instr){
			.syn = intel,
			.arch = mips32,
			.buf = raw,
			.len = sizeof(raw),
		},
		&enc
	);

	ir_raw_instr buf = init_ir_raw_instr(0);
	mips32_raw_to_ir(enc.mips32, &buf);

	return 0;
}

