#include "mte/core.h"
#include "mte/ir/ir.h"
#include "mte/perf.h"

#include "mte/asm/mips/mips32.h"
#include "mte/asm/intel/intel64.h"

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
	_in intel64_opcode 	opcode,
	_in u8 			modrm,
	_in intel64_sib 	sib,
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
	intel64_mte_raw_instr instr = b;

	__INL_PERF_INIT
	__INL_PERF_START

	volatile const intel64_opcode opcode = _intel64_get_opcode(instr);
	volatile u8 modrm = _intel64_get_modrm(opcode, instr);
	volatile const intel64_sib sib = _intel64_get_sib(opcode, modrm, instr);
	volatile u32 disp = _intel64_get_disp(opcode, modrm, instr);
	volatile u64 immd = _intel64_get_immd(opcode, modrm, instr);

	__INL_PERF_END

	printf("Empty in ns: %lf\n", (mm_perf_empty / 4.2));
	printf("Time in ns: %lf\n", (__INL_PERF_SUM / 4.2));
	printf("%02x\n", modrm);
	printf("%u %u %u %u\n", opcode.info.x, opcode.info.r, opcode.info.l, opcode.info.e);
	printf("%02x %02x %02x\n", opcode.val, opcode.legacy, opcode.rex);
	printf("%02x %02llx\n", disp, immd);
	printf("%02x\n", sib.val);
	printf("%02x\n", opcode.len);
}
// TEMPORARY
#include <windows.h>

#include "mte/pipe/vrow.h"
#include "mte/pipe/vrow_b0.h"
#include "mte/pipe/vrow_thread.h"
#include "mte/pipe/scheduler.h"

int main(){
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	//SetProcessAffinityMask(GetCurrentProcess(), 3);

	_intel64_prefetch_immd();
	_intel64_prefetch_modrm();

	_mips32_prefetch_reg_byte_table();
	_mips32_prefetch_op_byte_table();

	intel64_load_qtables();
	mips32_load_qtables();
	__asm__ __volatile__("mfence");

	axres res = AX_SUCC;

	/*
		Create single vertical row
	*/
	vrow_desc *vrow = nullptr;
	res = vrow_create(
		&vrow
	);
	axcheck(res, ax_log(res));

	/*
		Create IR context
	*/
	ir_context *ir = nullptr; 
	res = ir_create(
		IR_VER,
		MIPS32,
		INTEL64,
		&ir
	);
	axcheck(res, ax_log(res));

	mte_u64_instr instr_str =  _str_to_u64("add $t0, $t1, $t2", strlen("add $t0, $t1, $t2"));
	mte_raw_instr instr = {0};
	volatile axres r = mips32_byte_to_raw(
		&(mte_byte_instr){
			.syn = SYN_INTEL,
			.arch = MIPS32,
			.val = instr_str,
		},
		&instr
	);

	/*
		Load to bank 0
	*/

	sched_context *sched = nullptr;
	sched_create(ir, vrow, &sched);

	vrow_bank_thread b0 = init_vrow_bank_thread(sched, 0, vrow_b0_main);
	vrow_thread_start(&b0);
	struct vrow_b0_payload *b0_p = &init_vrow_b0_payload(ir, instr);

	/*vrow_bank_thread b1 = init_vrow_bank_thread(vrow, 0, vrow_b0_entry);
	vrow_thread_start(&b1);
	struct vrow_b0_payload *b1_p = &init_vrow_b0_payload(ir, instr);

	vrow_bank_thread b2 = init_vrow_bank_thread(vrow, 0, vrow_b0_entry);
	vrow_thread_start(&b2);
	struct vrow_b0_payload *b2_p = &init_vrow_b0_payload(ir, instr);

	vrow_bank_thread b3 = init_vrow_bank_thread(vrow, 0, vrow_b0_entry);
	vrow_thread_start(&b3);
	struct vrow_b0_payload *b3_p = &init_vrow_b0_payload(ir, instr);*/

	//__INL_PERF_INIT
	//__INL_PERF_START

	//__INL_PERF_END
	//__INL_PERF_LOG

	//ax_log(r);
	unref(r);
	axfree(instr_str.org);

	//io_i64(vrow->states);

	//io_i64(lock);
	//io_i64(_MEM_ACTIVE);

	//io_i64(atomic_load(&vrow->close));


	//vrow_alloc_action(vrow);
	__asm__ __volatile__("mfence");
	__INL_PERF_INIT
	__INL_PERF_START

	u32 i = 0;
	while(i++ < 10) {
		_mm_pause();
		vrow_bank_load(vrow, 0, *(vrow_payload*)b0_p);
	}
	vrow_delete(vrow);
	ir_delete(ir);

	__INL_PERF_END
	__INL_PERF_LOG
	__INL_PERF_LOG_MS

	/*
		Initialize bank 0 handling thread
	*/
	/*struct vrow_b0_payload b0 
		= init_vrow_b0_payload(ir, instr);
	vrow_bank_thread b0_thread 
		= init_vrow_bank_thread(vrow, 0, vrow_b0_entry, ((struct vrow_bank_thread_stack){.vrow = vrow, .bank = 0}));

	vrow_thread_start(&b0_thread);

	volatile bool lock
		= vrow_bank_load(vrow, 0, *(vrow_payload*)&b0);

	//__INL_PERF_LOG

	unref(lock);
	while(1){
		_mm_pause();
		vrow_bank_load(vrow, 0, *(vrow_payload*)&b0);
	}*/

	//struct vrow_b0_payload p = *(struct vrow_b0_payload*)vrow.base;
	//printf("%s", (u8*)&p.control.context->version);

	/*foo((u8[15]){0x66, 0x0f, 0x3a, 0x0e, 0xca, 0x0a});
	foo((u8[15]){0x0f, 0x3a, 0x0e, 0xca, 0x0a});
	foo((u8[15]){0x66, 0x0f, 0x3a, 0x0e, 0xca, 0x0a});
	foo((u8[15]){0x0f, 0x3a, 0x0e, 0xca, 0x0a});
	foo((u8[15]){0x66, 0x0f, 0x3a, 0x0e, 0xca, 0x0a});
	foo((u8[15]){0x0f, 0x3a, 0x0e, 0xca, 0x0a});
	foo((u8[15]){0x66, 0x0f, 0x3a, 0x0e, 0xca, 0x0a});
	foo((u8[15]){0x0f, 0x3a, 0x0e, 0xca, 0x0a});
	foo((u8[15]){0x66, 0x0f, 0x3a, 0x0e, 0xca, 0x0a});
	foo((u8[15]){0x0f, 0x3a, 0x0e, 0xca, 0x0a});
	foo((u8[15]){0x66, 0x0f, 0x3a, 0x0e, 0xca, 0x0a});
	foo((u8[15]){0x0f, 0x3a, 0x0e, 0xca, 0x0a});
	foo((u8[15]){0x66, 0x0f, 0x3a, 0x0e, 0xca, 0x0a});
	foo((u8[15]){0x0f, 0x3a, 0x0e, 0xca, 0x0a});
	foo((u8[15]){0x66, 0x0f, 0x3a, 0x0e, 0xca, 0x0a});
	foo((u8[15]){0x0f, 0x3a, 0x0e, 0xca, 0x0a});
	foo((u8[15]){0x48, 0xC7, 0xC1, 0xFF, 0x00, 0x00, 0x00});*/

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

