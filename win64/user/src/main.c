#include "mte/core.h"
#include "mte/ir/ir.h"
#include "mte/perf.h"

#include "mte/asm/mips/mips32.h"
#include "mte/asm/intel/intel64.h"

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

int main(){
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	SetProcessAffinityMask(GetCurrentProcess(), 1);

	_intel64_prefetch_immd();
	_intel64_prefetch_modrm();
	intel64_load_qtables();
	__asm__ __volatile__("mfence");

	vrow_desc vrow = {0};
	axcheck_r(init_vrow(&vrow), 0);
	vrow_desc *vrow_ref = &vrow; 

	__INL_PERF_INIT
	__INL_PERF_START

	volatile bool lock 
		= vrow_load(vrow_ref, (vrow_payload){.control = {10, 20, 30}, .data = {40, 50, 60}});

	__INL_PERF_END
	printf("Empty in ns: %lf\n", (mm_perf_empty / 4.2));
	printf("Time in ns: %lf\n", (__INL_PERF_SUM / 4.2));
	printf("%x\n", lock);

	io_i64(vrow.states);
	io_i64(vrow.base[0]);

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

