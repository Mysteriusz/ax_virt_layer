#include "mte/core.h"
#include "mte/ir.h"
#include "mte/asm/mips/mips32_asm.h"
#include "mte/asm/mips/mips32_ir.h"
#include "mte/asm/x86/x86_64.h"

#include "stdarg.h"

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
	_in x86_64_opcode 	opcode,
	_in u8 			modrm,
	_in x86_64_sib 		sib,
	_in u32 		disp 
){
	io_str(u"Opcode value:");
	printf(" - 0x%x\n", opcode.val);
	io_str(u"Legacy value:");
	printf(" - 0x%x\n", opcode.legacy);
	io_str(u"REX value:");
	printf(" - 0x%x\n", opcode.rex);
	printf("  - W: %u\n", rex_w(opcode.rex));
	printf("  - R: %u\n", rex_r(opcode.rex));
	printf("  - X: %u\n", rex_x(opcode.rex));
	printf("  - B: %u\n", rex_b(opcode.rex));
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
	io_str(u"Base register:");
	io_str(val_to_reg((rex_r(opcode.rex) << 3) | modrm_reg(modrm)));
	io_str(u"Scale register:");
	io_str(val_to_reg((sib.index_ext << 3) | sib.index));
	io_str(u"Base register:");
	io_str(val_to_reg((sib.base_ext << 3) | sib.base));
}
_inline_avert void foo(
	u8 b[15]
){
	x86_64_mte_raw_instr instr = b;
	u64 l1, l2;

	(void)__rdtsc();
	_mm_lfence();
	l1 = __rdtsc();
	_mm_lfence();
	const x86_64_opcode opcode = _x86_64_get_opcode(instr);
	u8 modrm = _x86_64_get_modrm(opcode, instr);
	const x86_64_sib sib = _x86_64_get_sib(opcode, modrm, instr);
	u32 disp = _x86_64_get_disp(opcode, modrm, instr);
	_mm_lfence();
	l2 = __rdtsc();
	//instr_dis(opcode, modrm, (x86_64_sib){0}, 0);
	//instr_dis(opcode, modrm, sib, disp);
	printf("Time in ns: %lf\n", ((l2 - l1) / 4.2) - 4);
	printf("%u, %u, %u, %u\n", opcode.val, modrm, sib.val, disp);
}
int main(){
	_x86_64_prefetch_modrm();
	/*x86_64_mte_raw_instr instr1 = init_x86_64_mte_raw_instr(0x66, 0x45, 0x0f, 0x38, 0x00);
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
	foo(instr6);*/
	//foo((u8[15]){0x48, 0x01, 0x08});
	//foo((u8[15]){0x04, 0x02});
	/*foo((u8[15]){0x00, 0x08});
	foo((u8[15]){0x48, 0x01, 0x08});
	foo((u8[15]){0x02, 0x08});
	foo((u8[15]){0x48, 0x03, 0x08});
	foo((u8[15]){0x04, 0x01});
	foo((u8[15]){0x04, 0x01});
	foo((u8[15]){0x05, 0xff, 0xff, 0x00, 0x00});)*/
	//foo((u8[15]){0x8B, 0x44, 0x8B, 0x10});
	//foo((u8[15]){0x8B, 0x94, 0xF8, 0x78, 0x56, 0x34, 0x12});
	//foo((u8[15]){0x8B, 0x94, 0xF8, 0x78, 0x56, 0x34, 0x12});
	//foo((u8[15]){0x48, 0x8D, 0xB4, 0x55, 0x00, 0x04, 0x00, 0x00});
	//foo((u8[15]){0x48, 0x8D, 0xB4, 0x55, 0x00, 0x04, 0x00, 0x00});
	foo((u8[15]){0x43, 0x01, 0x84, 0x1A, 0xFF, 0xFF, 0xFF, 0x7F});
	foo((u8[15]){0x8B, 0x83, 0x44, 0x33, 0x22, 0x11});
	foo((u8[15]){0x2B, 0x8C, 0x24, 0x88, 0x13, 0x00, 0x00});
	foo((u8[15]){0x41, 0x83, 0xBF, 0xEE, 0xDB, 0xEA, 0x0D, 0x00});
	foo((u8[15]){0x83, 0xBA, 0x00, 0x10, 0x00, 0x00, 0x00});
	foo((u8[15]){0x4E, 0x03, 0x04, 0x08});
	//instr_dis((u8[15]){0x41, 0x83, 0xBF, 0x00, 0x10, 0x00, 0x00, 0x00});
	//foo((u8[15]){0x4E, 0x03, 0x04, 0x08});
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

