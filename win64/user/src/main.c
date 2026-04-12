#include "mte/core.h"
#include "mte/ir/ir.h"
#include "mte/perf.h"

#include "mte/asm/mips/mips32.h"
#include "mte/asm/intel/i64.h"

#include <stdarg.h>
#include <intrin.h>

// TEMPORARY
#include <windows.h>

#include "mte/pipe/vrow.h"
#include "mte/pipe/vrow_bank.h"
#include "mte/pipe/sched.h"
#include "mte/pipe/bitpool.h"

#include "intel/emitter/i64_emit.h"
#include "intel/emitter/test_suite.h"
#include "mte/ir/tblock.h"

int main(){
	//SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	SetThreadAffinityMask(GetCurrentThread(), 1);

	/*for (u32 i = 0; i < 0xff; i += (64 / sizeof(i64_opcode_desc))){
		_mm_prefetch(&L1_OPCODE_META_TABLE[i], _MM_HINT_T0);
	}
	init_suite();*/
	_mm_mfence();

	axres res = 0;

#if 0
	u8 instr_hold[16] = {0};
	u64 val = 0;
	srand(time(NULL));
	u32 indices[100000];
	for (u32 i = 0; i < 100000; i++){
        	indices[i] = rand() % 30;
	}

	__INL_PERF_INIT
	__INL_PERF_START
	for (u32 i = 0; i < 100000; i++){
		enum i64_opcode op = add_opcodes[indices[i]];
		i64_operand *ops = add_cases[indices[i]];
		res = i64_emit_64(
			op,
			ops,
			instr_hold
		);
		val += *(u64*)&instr_hold[0] + *(u64*)&instr_hold[8];
	}
	__INL_PERF_END

	printf("Average time in ns: %lf\n", ((double)__INL_PERF_SUM / 100000) / 4.2);
	io_u64(val);
#endif
#if 0 
	mips32_load_qtables();
	init_suite();

	ir_context *ir;
	res = ir_create(IR_VER, MIPS32, INTEL64, &ir);
	axcheck(res, ax_log(res));

	ir->desc.code_base = axmalloc(GIB(1));
	ir->desc.gen_base = axmalloc(GIB(1));

	u8 instr_hold[16];
	__INL_PERF_INIT
	__INL_PERF_START
	for (u32 i = 0; i < 1; i++){
		enum i64_opcode op = add_opcodes[0];
		i64_operand *ops = add_cases[0];
		u8 len = 0;
		res = i64_emit_64(
			op,
			ops,
			instr_hold,
			&len
		);
		for (u8 j = len - 1; j > 0; j--){
			printf("%02x", instr_hold[j]);
		}
		printf("%02x", instr_hold[0]);
	}
	__INL_PERF_END

	//printf("Average time in ns: %lf\n", ((double)__INL_PERF_SUM / 100000) / 4.2);

#endif
#if 0
	u8 len1 = 0;
	
	__INL_PERF_INIT
	__INL_PERF_START
	ir->desc.call.ir_to_tar(
		(ir_raw_instr){
			.opcode = IR_ADD_I32,
			.set = {
				.ops_count = 3,
				.ops = {
					(ir_operand){.id = IR_OP_REG,.value = I64_rAX & 0xf},
					(ir_operand){.id = IR_OP_REG,.value = I64_rBX & 0xf},
					(ir_operand){.id = IR_OP_REG,.value = I64_rCX & 0xf},
				}
			}
		},
		ir,
		&len1
	);
	u8 len2 = 0;
	ir->desc.call.ir_to_tar(
		(ir_raw_instr){
			.opcode = IR_ADD_I8,
			.set = {
				.ops_count = 3,
				.ops = {
					(ir_operand){.id = IR_OP_REG,.value = I64_rAX},
					(ir_operand){.id = IR_OP_REG,.value = I64_rAX},
					(ir_operand){.id = IR_OP_REG,.value = I64_rBX},
				}
			}
		},
		ir,
		&len2
	);
	__INL_PERF_END
	__INL_PERF_LOG
	//io_i64(len1);
	//io_i64(len2);
#endif
#if 1
	ir_context *ir;
	res = ir_create(IR_VER, MIPS32, INTEL64, &ir);
	axcheck(res, ax_log(res));

	ir->desc.code_base = axmalloc(GIB(1));
	ir->desc.gen_base = axmalloc(GIB(1));


	//((u32*)ir->desc.code_base)[0] = 0x01896020; // add $t4, $t4, $t1
	((u32*)ir->desc.code_base)[0] = 0x01896020; // add $t4, $t4, $t1
	((u32*)ir->desc.code_base)[1] = 0x012A4820; // add $t1, $t1, $t2

	ir->desc.gen_ptr = (u8*)ir->desc.gen_base;
	ir->desc.code_ptr = (u8*)ir->desc.code_base;

	tblock tblock = {0};

	tblock_alloc(ir, TBLOCK_SMALL, &tblock);

	bool emit = tblock_emit(&tblock);
	//emit = tblock_emit(&tblock);

	u8 i = 0;
	while(i < 20){
		printf("%02x", ir->desc.gen_base[i++]);
	}
#endif

	//printf("Time in ns per instruction: %lf\n", (__INL_PERF_SUM / 4.2) / 128);
	
	/*printf("%i\n", emit);
	printf("%i\n", ir->desc.gen_ptr[-1]);
	printf("%i\n", ir->desc.gen_base[0]);*/

	return 0;
}

