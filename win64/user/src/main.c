#include "mte/core.h"
#include "mte/ir/ir.h"
#include "mte/perf.h"

#include "mte/asm/mips/mips32.h"
#include "mte/asm/intel/i64.h"

#include "mte/asm/mips/mips32_asm.h"

#include <stdarg.h>
#include <intrin.h>

static const c16 *val_to_reg(
	_in u8 val
){
	static const c16* names[] = {
		u"rax", u"rcx", u"rdx", u"rbx", 
		u"rsp", u"rbp", u"rsi", u"rdi",
		u"r8",  u"r9",  u"r10", u"r11", 
		u"r12", u"r13", u"r14", u"r15"
	};
	
	if (val < 16) {
		return names[val];
	}
	return u"";
}
static void instr_dis(
	//_in i64_opcode 	opcode,
	_in u8 			modrm,
	//_in i64_sib 	sib,
	_in u32 		disp,
	_in u64 		immd
){
	//__asm__ __volatile__("" :: "g"(opcode), "g"(modrm), "g"(sib), "g"(disp), "g"(immd));
	/*io_str(u"Opcode value:");
	printf(" - 0x%x\n", opcode.val);
	io_str(u"Legacy value:");
	printf(" - 0x%x\n", opcode.legacy);
	io_str(u"REX value:");
	printf(" - 0x%x\n", opcode.rex);
	printf("  - W: %u\n", rex_w(opcode.rex));
	printf("  - R: %u\n", rex_r(opcode.rex));
	printf("  - X: %u\n", rex_x(opcode.rex));
	printf("  - B: %u\n", rex_b(opcode.rex));
	io_str(u"Base register:");
	io_i64(modrm_reg(modrm));
	io_str(val_to_reg((rex_r(opcode.rex) << 3) | modrm_reg(modrm)));
	if (modrm != 0){
		io_str(u"MODRM value:");
		printf(" - 0x%x\n", modrm);
		printf("  - MOD: %u\n", modrm_mod(modrm));
		printf("  - REG: %u\n", modrm_reg(modrm));
		printf("  - R/M: %u\n", modrm_rm(modrm));
		io_str(u"SIB value:");
		printf(" - 0x%x\n", sib.val);
		printf("  - SCALE: %u\n", sib_scale(sib.val));
		printf("  - INDEX: %u\n", (sib.index_ext << 3) | sib.index);
		printf("  - BASE: %u\n", (sib.base_ext << 3) | sib.base);
		io_str(u"Scale register:");
		io_str(val_to_reg((sib.index_ext << 3) | sib.index));
		io_str(u"Base register:");
		io_str(val_to_reg((sib.base_ext << 3) | sib.base));
	}
	printf("  - Displacement: %02x\n", disp);
	printf("  - Immediate: %02llx\n", immd);*/
}


_inline_avert void foo(
	u8 b[15]
){
	__INL_PERF_INIT
	//i64_mte_raw_instr instr = b;

	__INL_PERF_START

	/*volatile const i64_opcode opcode = _i64_get_opcode(instr);
	volatile u8 modrm = _i64_get_modrm(opcode, instr);
	volatile const i64_sib sib = _i64_get_sib(opcode, modrm, instr);
	volatile u32 disp = _i64_get_disp(opcode, modrm, instr);
	volatile u64 immd = _i64_get_immd(opcode, modrm, instr);*/

	__INL_PERF_END
	__INL_PERF_LOG

	/*printf("%02x\n", modrm);
	printf("%u %u %u %u\n", opcode.info.x, opcode.info.r, opcode.info.l, opcode.info.e);
	printf("%02x %02x %02x\n", opcode.val, opcode.legacy, opcode.rex);
	printf("%02x\n", opcode.len);
	printf("%02x %02llx\n", disp, immd);
	printf("%02x\n", sib.val);*/
}
// TEMPORARY
#include <windows.h>

#include "mte/pipe/vrow.h"
#include "mte/pipe/vrow_bank.h"
#include "mte/pipe/sched.h"
#include "mte/pipe/bitpool.h"

#include "intel/emitter/i64_emit.h"
#include "intel/emitter/test_suite.h"

int main(){
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	SetThreadAffinityMask(GetCurrentThread(), 1);

	for (u32 i = 0; i < 0xff; i += (64 / sizeof(i64_opcode_desc))){
		_mm_prefetch(&L1_OPCODE_META_TABLE[i], _MM_HINT_T0);
	}
	init_suite();
	_mm_mfence();

	simd_128 buf = {0};
	axres res = 0;
	u8 instr_hold[16] = {0};
	u64 sum = 0;
	u64 val = 0;
	u32 r = 0;
	srand(time(NULL));
	u32 indices[100000];
	for (u32 i = 0; i < 100000; i++){
        	indices[i] = rand() % 30;
	}

	for (u32 i = 0; i < 100000; i++){
		enum i64_opcode op = add_opcodes[indices[i] % 30];
		i64_operand *ops = add_cases[indices[i] % 30];
	__INL_PERF_INIT
	__INL_PERF_START
		res = i64_emit_64(
			op,
			ops,
			instr_hold
		);
	__INL_PERF_END
		sum += __INL_PERF_SUM;
		val += *(u64*)&instr_hold[0] + *(u64*)&instr_hold[8];
	}

	printf("Average time in ns: %lf\n", ((double)sum / 100000) / 4.2);
	io_u64(val);
}

