#if !defined(MTE_MIPS32_REG_TABLE_INT)
#define MTE_MIPS32_REG_TABLE_INT

#include "mips/mips32_instr.h"

/*
	MIPS32-specific lookup tables provided by GNU gpref
*/

struct mips32_reg_byte_info{
	const u64		name_u64;
	const enum mips32_reg 	val;
};

_unused
static const struct mips32_reg_byte_info _mips32_reg_byte_table[] = {
	{0}, {0},
	{ .name_u64=0/*$2*/},
	{ .name_u64=0/*$28*/},
	{ .name_u64 = 0x3824/*$8*/, 	.val = R8 },
	{ .name_u64=0/*$25*/},
	{0},
	{ .name_u64=0/*$1*/},
	{ .name_u64=0/*$18*/},
	{0},
	{ .name_u64=0/*$15*/},
	{0},
	{ .name_u64=0/*$0*/},
	{ .name_u64=0/*$s8*/},
	{0},
	{ .name_u64=0/*$s5*/},
	{0}, {0},
	{ .name_u64=0/*$t8*/},
	{0},
	{ .name_u64=0/*$t5*/},
	{0}, {0},
	{ .name_u64=0/*$sp*/},
	{ .name_u64=0/*$7*/},
	{ .name_u64=0/*$24*/},
	{0},
	{ .name_u64=0/*$3*/},
	{ .name_u64=0/*$20*/},
	{0},
	{ .name_u64=0/*$14*/},
	{0}, {0},
	{ .name_u64 = 0x303124/*$10*/,	.val = R10 },
	{0},
	{ .name_u64=0/*$s4*/},
	{0}, {0},
	{ .name_u64=0/*$s0*/},
	{0},
	{ .name_u64=0/*$t4*/},
	{0}, {0},
	{ .name_u64=0x307424/*$t0*/, 	.val = R8 },
	{0},
	{ .name_u64=0/*$27*/},
	{0}, {0},
	{ .name_u64=0/*$u0*/},
	{0},
	{ .name_u64=0/*$17*/},
	{0}, {0},
	{ .name_u64=0/*$30*/},
	{0},
	{ .name_u64=0/*$s7*/},
	{0},
	{ .name_u64=0/*$4*/},
	{ .name_u64=0/*$21*/},
	{0},
	{ .name_u64=0/*$t7*/},
	{0}, {0},
	{ .name_u64 = 0x313124/*$11*/, 	.val = R11 },
	{0},
	{ .name_u64=0/*$29*/},
	{0}, {0},
	{ .name_u64=0/*$s1*/},
	{0},
	{ .name_u64=0/*$19*/},
	{0}, {0},
	{ .name_u64 = 0x317424/*$t1*/, 	.val = R9 },
	{0},
	{ .name_u64=0/*$zero*/},
	{0}, {0},
	{ .name_u64=0/*$u1*/},
	{0},
	{ .name_u64=0/*$t9*/},
	{0}, {0},
	{ .name_u64=0/*$31*/},
	{0},
	{ .name_u64=0/*$gp*/},
	{0},
	{ .name_u64=0/*$6*/},
	{ .name_u64=0/*$23*/},
	{0},
	{ .name_u64=0/*$fp*/},
	{0}, {0},
	{ .name_u64=0/*$13*/},
	{0}, {0}, {0}, {0},
	{ .name_u64=0/*$s3*/},
	{0}, {0}, {0}, {0},
	{ .name_u64 = 0x337424/*$t3*/, 	.val = R11 },
	{0}, {0}, {0},
	{ .name_u64=0/*$5*/},
	{ .name_u64=0/*$22*/},
	{0}, {0}, {0}, {0},
	{ .name_u64=0/*$12*/},
	{0}, {0}, {0}, {0},
	{ .name_u64=0/*$s2*/},
	{0}, {0}, {0}, {0},
	{ .name_u64 = 0x327424/*$t2*/, 	.val = R10 },
	{0}, {0}, {0},
	{ .name_u64 = 0x3924/*$9*/,	.val = R9 },
	{ .name_u64=0/*$26*/},
	{0}, {0}, {0}, {0},
	{ .name_u64=0/*$16*/},
	{0}, {0}, {0}, {0},
	{ .name_u64=0/*$s6*/},
	{0}, {0}, {0}, {0},
	{ .name_u64=0/*$t6*/},
	{0}, {0}, {0}, {0},
	{ .name_u64=0/*$a0*/},
	{0}, {0}, {0}, {0},
	{ .name_u64=0/*$k0*/},
	{0}, {0}, {0}, {0},
	{ .name_u64=0/*$at*/},
	{0}, {0}, {0}, {0},
	{ .name_u64=0/*$ra*/},
	{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
	{0}, {0}, {0}, {0}, {0},
	{ .name_u64=0/*$a1*/},
	{0}, {0}, {0}, {0},
	{ .name_u64=0/*$k1*/},
	{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
	{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
	{0}, {0}, {0}, {0}, {0}, {0},
	{ .name_u64=0/*$a3*/},
	{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
	{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
	{0},
	{ .name_u64=0/*$a2*/}
};

static void _mips32_prefetch_reg_byte_table(
	void
){
	for (u8 i = 0; i < (sizeof(_mips32_reg_byte_table) / 64); i++){
		// Given size of the table prefetch to L2 to preserve L1 tables
		_mm_prefetch(offp(_mips32_reg_byte_table, (i << 6)), _MM_HINT_T1);
	}
};

#endif // !defined(MTE_MIPS32_REG_TABLE_INT)

