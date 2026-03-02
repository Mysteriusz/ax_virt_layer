#include <ax_io.h>

#include "mte/perf.h"

#include "i64_emit_info.h"
#include "i64_emit.h"

struct i64_operand_sum i64_sum_calc(
	_in i64_opcode_desc	desc,
	_in i64_operand		ops[I64_RED_OP_COUNT]
){
	if (ops == nullptr){
		return (struct i64_operand_sum){0};
	}

	struct i64_operand_sum sum = {0};
	u8 w0 = (1 << 3); // D
	u8 w1 = 0; // A
	u8 w2 = 0; // E

	u8 o0 = 0; // F and E
	u8 o1 = 0; // D and C
	u8 o2 = 0; // B and A

	u8 sib_i = 0;
	i64_operand *t0 = &ops[0];
	i64_operand *t1 = &ops[1];
	bool t0_ext = (t0->id & I64_OP_EXT);
	bool t0_mem = (t0->desc.type & I64_MEM);

	bool t1_ext = (t1->id & I64_OP_EXT);
	bool t1_mem = (t1->desc.type & I64_MEM);

	for (u32 i = 0; i < desc.op_count; i++){
		enum i64_operand_id ii = ops[i].id;
		enum i64_operand_type ti = ops[i].desc.type;
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

		// If is a memory operand and width is 32-bit
		w2 |= (w2m0 & w2m1 & BIT(4));

		// If is a memory operand and [index] of the memory is extended
		o0 |= (o0m0 & o0m1 & BIT(5));

		// If is 32-bit displacement
		w0 |= (w0m1 & BIT(6));

		// If is 8-bit displacement
		w0 |= (w0m0 & BIT(5));

		// If is a memory operand
		o0 |= (o0m3 & BIT(6));

		sib_i = i * !!(o0 & BIT(4));
	}

	o1 = (t0_ext && !t0_mem) << 3;
	o2 = (t1_ext && !t1_mem) << 1;
	o1 = (t0_ext && t0_mem) << 2;
	o2 = (t1_ext && t1_mem);

	sum.width = w0 | w1 | w2;
	sum.operand = o0 | o1 | o2;
	sum.sib_i = sib_i;

	return sum;
}

#include "i64_emit.h"
u64 in_i = 0;
u64 in_n = 0;
axres i64_emit_64(
	_in enum i64_opcode 		opcode,
	_in i64_operand 		ops[I64_RED_OP_COUNT],
	_out i64_mte_raw_instr		*buf
){
	__INL_PERF_INIT
	__INL_PERF_START
	if (ops == nullptr){
		return AX_INV_ARG;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	// Read most significant byte of the opcode (length) based on [MTE_I64_OPCODE_INT.I64_OI]
	u8 opcode_len = ((opcode >> 56) & 0xff);
	if (opcode_len > I64_MAX_OPCODE_LEN){
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

//__INL_PERF_START
	struct i64_operand_sum sum =
		i64_sum_calc(opcode_desc, ops);
//__INL_PERF_END

	/*
	 	Evaluate SIB-specific field references
	*/

	i64_operand *sib_op =
		&ops[sum.sib_i];
	struct i64_operand_mem *sib_mem =
		(struct i64_operand_mem*)&sib_op->value;
		
	/*
	   	TODO: ADD CASE WHERE 2 OPERAND REGISTERS ARE NOT THE SAME WIDTH
	 	
	 	Check opcode operands and fill [opcode_match] descriptors
	*/

	i64_mte_raw_instr instr = {0};

	/*
		Resolve REX
	*/

	volatile u8 rex = 
		_i64_rex_resolve(sum);

	/*
		Resolve Legacy prefix

		TODO: Opcodes may have mandatory prefix
		that is a part of it (access by opocde_desc.prefix)

		Both legacy prefixes should then be used.
		
		Example:
			adcx rax, [ebx]	(ADX instruction set)
	*/

	volatile u8 leg = 
		_i64_leg_resolve(sum);

	/*
		Resolve MODRM (Only if applicable to the opcode)
	*/

	volatile u8 modrm = 0;
	if (opcode_desc.flags & MODRM){
		modrm = _i64_modrm_resolve(ops[0].id, ops[1].id, sum);
	}

	volatile u8 sib = 
		_i64_sib_resolve(
			sib_op->id,
			*(struct i64_operand_mem*)&sib_op->value,
			sum);

	*buf = instr;

	__INL_PERF_END
	__INL_PERF_LOG

	io_str(u"LEGACY VALUE:");
	printf("%x\n", sum.operand);
	printf("%x\n", sum.sib_i);
	printf("%x\n", sum.width);
	io_str(u"LEGACY VALUE:");
	printf("%x\n", leg);
	io_str(u"REX VALUE:");
	printf("%x\n", rex);
	io_str(u"MODRM VALUE:");
	printf("%x\n", modrm);
	io_str(u"SIB VALUE:");
	printf("%x\n", sib);
	io_str(u"ALT:");
	printf("%x %x %x %x %x %x\n", leg, rex, (u8)(opcode & 0xff), modrm, sib, sib_mem->disp);

	in_i += __INL_PERF_SUM;
	in_n++;
	return AX_SUCC;
}

