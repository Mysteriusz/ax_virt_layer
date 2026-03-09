#include "mips32_ir.h"
#include "mips32_instr.h"
#include "mte/perf.h"

axres mips32_raw_to_ir(
	_in mips32_mte_raw_instr	instr,
	_in_out ir_raw_instr		*buf
){

	if (__builtin_expect(buf == nullptr, false)){
		return AX_INV_BUF;
	}

	switch(_mips32_get_instr_type(instr)){
	case R:
		buf->opcode = MIPS32_TO_IR_TABLE[_mips32_funct(instr)];
		buf->regs[0] = (ir_operand){
			.id = IR_OP_REG,
			.value = _mips32_rd(instr)
		};
		buf->regs[1] = (ir_operand){
			.id = IR_OP_REG,
			.value = _mips32_rs(instr)
		};
		buf->regs[2] = (ir_operand){
			.id = IR_OP_REG,
			.value = _mips32_rt(instr)
		};
		buf->reg_used = 3;
		break;
	case I:
		break;
	case J:
		break;
	default:
		return AX_MTE_INV_INSTR;
	}

	return AX_SUCC;
}

ir_raw_instr mips32_to_ir_call(
	_in mte_raw_instr	instr,	
	_in ir_context		*context
){
	ir_raw_instr buf = {0};
	axres res = mips32_raw_to_ir(instr.payload[0], &buf);
	axcheck_r(res, (ir_raw_instr){0});

	return buf;
}

#include "mte/cpu.h"
struct cpu_reg_map _MIPS32_CPU_REG_MAP = {
	.reg_count = 32, // 32 registers r0-r31
	.reg_width = 32, // 32-Bit architecture
	.root = (struct cpu_reg_desc[32]){
		[0]  = {.id = R0, 	.role = REG_PRESERVE,  .value = 0},
		[1]  = {.id = R1, 	.role = REG_TEMP,      .value = 0},
		[2]  = {.id = R2, 	.role = REG_RETURN,    .value = 0},
		[3]  = {.id = R3, 	.role = REG_RETURN,    .value = 0},
		[4]  = {.id = R4, 	.role = REG_STACK,     .value = 0},
		[5]  = {.id = R5, 	.role = REG_STACK,     .value = 0},
		[6]  = {.id = R6, 	.role = REG_STACK,     .value = 0},
		[7]  = {.id = R7, 	.role = REG_STACK,     .value = 0},
		[8]  = {.id = R8, 	.role = REG_TEMP,      .value = 0},
		[9]  = {.id = R9, 	.role = REG_TEMP,      .value = 0},
		[10] = {.id = R10, 	.role = REG_TEMP,      .value = 0},
		[11] = {.id = R11, 	.role = REG_TEMP,      .value = 0},
		[12] = {.id = R12, 	.role = REG_TEMP,      .value = 0},
		[13] = {.id = R13, 	.role = REG_TEMP,      .value = 0},
		[14] = {.id = R14, 	.role = REG_TEMP,      .value = 0},
		[15] = {.id = R15, 	.role = REG_TEMP,      .value = 0},
		[16] = {.id = R16, 	.role = REG_PRESERVE,  .value = 0},
		[17] = {.id = R17, 	.role = REG_PRESERVE,  .value = 0},
		[18] = {.id = R18, 	.role = REG_PRESERVE,  .value = 0},
		[19] = {.id = R19, 	.role = REG_PRESERVE,  .value = 0},
		[20] = {.id = R20, 	.role = REG_PRESERVE,  .value = 0},
		[21] = {.id = R21, 	.role = REG_PRESERVE,  .value = 0},
		[22] = {.id = R22, 	.role = REG_PRESERVE,  .value = 0},
		[23] = {.id = R23, 	.role = REG_PRESERVE,  .value = 0},
		[24] = {.id = R24, 	.role = REG_TEMP,      .value = 0},
		[25] = {.id = R25, 	.role = REG_TEMP,      .value = 0},
		[26] = {.id = R26, 	.role = REG_PRESERVE,  .value = 0},
		[27] = {.id = R27, 	.role = REG_PRESERVE,  .value = 0},
		[28] = {.id = R28, 	.role = REG_FRAME_PTR, .value = 0},
		[29] = {.id = R29, 	.role = REG_STACK_PTR, .value = 0},
		[30] = {.id = R30, 	.role = REG_MEM_PTR,   .value = 0},
		[31] = {.id = R31, 	.role = REG_RETURN,    .value = 0},
	},
};

