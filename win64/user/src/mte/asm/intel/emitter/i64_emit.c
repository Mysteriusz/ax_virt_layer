#include <ax_io.h>

#include "mte/perf.h"

#include "i64_emit_info.h"
#include "i64_emit.h"

struct i64_operand_sum i64_sum_calc(
	_in i64_opcode_desc	desc,
	_in i64_operand		ops[I64_MAX_OP_COUNT]
){
	if (ops == nullptr){
		return (struct i64_operand_sum){0};
	}

	struct i64_operand_sum sum = {0};
	i64_operand *restrict t0 = &ops[0];
	i64_operand *restrict t1 = &ops[1];

	u8 w0 = (1 << 3); // D
	u8 w1 = 0; // A
	u8 w2 = 0; // E

	u8 o0 = 0; // H and F and E
	u8 o1 = 0; // D and C
	u8 o2 = 0; // B and A

	u8 sib_i = 0;

	/*
	 	000000EM
		E -> Extended operand
		M -> Memory operand
	*/
	u8 t0_em = (!!(t0->id & I64_OP_EXT)) << 1 | (t0->desc.type & I64_MEM);
	u8 t1_em = (!!(t1->id & I64_OP_EXT)) << 1 | (t1->desc.type & I64_MEM);

	o0 = (t0_em & BIT(0)) << 7;

	// Comply with canonical addressing (src == register, dest == memory)
	u8 dest = (t1_em & BIT(0)) ? t1_em : t0_em;
	u8 src = (t1_em & BIT(0)) ? t0_em : t1_em;

	for (u32 i = 0; i < (const u8)desc.op_count; i++){
		enum i64_operand_id ii = ops[i].id;
		enum i64_operand_type ti = ops[i].desc.type;
		// Processed operand width
		u8 wi = ops[i].desc.width;

		/*
		 	DONT CHANGE ANY BIT-SHIFTS

			No idea why exactly but it decreases performance
		*/

		u8 w0m0 = -((ti & I64_DISP8) == I64_DISP8);
		u8 w0m1 = -((ti & I64_DISP32) == I64_DISP32);

		u8 w2m0 = -((ti & I64_MEM) == I64_MEM);
		u8 w2m1 = -(wi == W32);

		u8 o0m0 = -((ti & I64_SIB) == I64_SIB);
		u8 o0m1 = -((ii & I64_OP_EXT) == I64_OP_EXT);
		u8 o0m2 = -((ti & I64_SIB) == I64_SIB);
		u8 o0m3 = -((ti & I64_MEM) == I64_MEM);

		// If operand width is 64-bit OR a memory operand
		w0 &= ~BIT(3) | (((wi == W64) | !!(ti & I64_MEM)) << 3);

		// If SIB addressing
		o0 |= (o0m2 & BIT(4));

		// If operand width is 16-bit
		w1 |= (wi == W16);

		// If is a memory operand and [index] of the memory is extended
		o0 |= (o0m0 & o0m1 & BIT(5));

		// If is a memory operand and width is 32-bit
		w2 |= (w2m0 & w2m1 & BIT(4));

		// If is 32-bit displacement
		w0 |= (w0m1 & BIT(6));

		// If is a memory operand
		o0 |= (o0m3 & BIT(6));

		// If is 8-bit displacement
		w0 |= (w0m0 & BIT(5));

		sum.sib_i = (o0 & BIT(4)) 
			? i
			: sum.sib_i;
	}

	bool dest_mem = dest & BIT(0);
	bool src_mem = src & BIT(0);

	bool dest_ext = dest & BIT(1);
	bool src_ext = src & BIT(1);
	
	// Destination register extended
	o1 = (dest_ext && !dest_mem) << 3;

	// Source register extended
	o2 = (src_ext && !src_mem) << 1;

	// Destination memory extended
	o1 |= (dest_ext && dest_mem) << 2;

	// Source memory extended
	o2 |= (src_ext && src_mem);
	
	sum.width = w0 | w1 | w2;
	sum.operand = o0 | o1 | o2;
	sum.sib_i = sib_i;

	return sum;
}

#include "i64_emit.h"
axres i64_emit_64(
	_in enum i64_opcode 		opcode,
	_in i64_operand 		ops[I64_MAX_OP_COUNT],
	_in_out u8			buf[16]
){
	// Read most significant byte of the opcode (length) based on [MTE_I64_OPCODE_INT.I64_OI]
	u8 opcode_len = ((opcode >> 56) & 0xff);

	if (__builtin_expect(ops == nullptr, false)){
		return AX_INV_ARG;
	}
	if (__builtin_expect(buf == nullptr, false)){
		return AX_INV_BUF;
	}
	if (__builtin_expect(opcode_len > I64_MAX_OPCODE_LEN, false)){
		return AX_INV_CODE;
	}

	i64_opcode_desc opcode_desc = {0};
	switch(opcode_len){
	case 1:
		// Map [opcode] to opcode desc table for 1 byte opcodes
		opcode_desc = L1_OPCODE_META_TABLE[opcode & 0xff];
		break;
	default:
		return AX_INV_DATA;
	}

	struct i64_operand_sum sum =
		i64_sum_calc(opcode_desc, ops);

	/*
	 	Evaluate SIB-specific field references
	*/

	u8 opcode_p0 = (opcode >> 16) & 0xff;
	u8 opcode_p1 = (opcode >> 8) & 0xff;
	u8 opcode_p2 = opcode & 0xff;
		
	/*
	   	TODO: ADD CASE WHERE 2 OPERAND REGISTERS ARE NOT THE SAME WIDTH
	 	
	 	Check opcode operands and fill [opcode_match] descriptors
	*/

	/*
		Resolve REX
	*/

	u8 rex = _i64_rex_resolve(sum);

	i64_operand *sib_op =
		&ops[sum.sib_i];

	struct i64_operand_mem *sib_mem =
		(struct i64_operand_mem*)&sib_op->value;

	/*
		Resolve Legacy prefix

		TODO: Opcodes may have mandatory prefix
		that is a part of it (access by opocde_desc.prefix)

		Both legacy prefixes should then be used.
		
		Example:
			adcx rax, [ebx]	(ADX instruction set)
	*/

	u8 leg = _i64_leg_resolve(sum);

	/*
		Resolve MODRM (Only if applicable to the opcode)
	*/

	u8 modrm = 0;
	if (opcode_desc.flags & MODRM){
		modrm = _i64_modrm_resolve(ops[0].id, ops[1].id, sum);
	}

	u8 sib = _i64_sib_resolve(
		sib_op->id,
		*sib_mem,
		sum);

	u8 disp_mode = (sum.width >> 5) & 0b11;

	bool present[16] = {0};
	present[11] = 0;
	present[10] = !!leg;
	present[9] = !!rex;
	present[8] = !!opcode_p0;
	present[7] = !!opcode_p1;
	present[6] = 1;
	present[5] = !!modrm;
	present[4] = !!sib;
	present[3] = (disp_mode & 3); // disp8 or disp32
	present[2] = (disp_mode & 2); // disp32
	present[1] = (disp_mode & 2); // disp32
	present[0] = (disp_mode & 2); // disp32

	u8 hold[16] = {0};
	hold[11] = 0; // Reserved for additional prefix like 0xf0
	hold[10] = leg;
	hold[9] = rex;
	hold[8] = opcode_p0;
	hold[7] = opcode_p1;
	hold[6] = opcode_p2; 
	hold[5] = modrm;
	hold[4] = sib;
	hold[3] = sib_mem->disp & 0xff;
	hold[2] = (sib_mem->disp >> 8) & 0xff;
	hold[1] = (sib_mem->disp >> 16) & 0xff;
	hold[0] = (sib_mem->disp >> 24) & 0xff;

	u8 n = 0;
	u8 i0 = 0, i0n = 0;
	for (;n < 16; i0++){
		buf[i0n] = hold[i0] * present[i0];
		i0n += present[i0];
		n++;
	}

	return AX_SUCC;
}

