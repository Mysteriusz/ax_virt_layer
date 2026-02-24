#include "mte/perf.h"

#include "i64_emit_info.h"
#include <ax_io.h>

struct i64_operand_sum i64_sum_calc(
	_in i64_opcode_desc	desc,
	_in i64_operand		ops[I64_RED_OP_COUNT]
){
	if (ops == nullptr){
		return (struct i64_operand_sum){0};
	}

	struct i64_operand_sum sum = {0};

	// Preset as true since it`s more efficient to AND on every operand
	sum.is_64bit = true;

	for (u32 i = 0; i < desc.op_count; i++){
		// Check operand compatiblity with desc
		if (!_i64_operand_cmp(desc.ops[i], ops[i].desc)){
			return (struct i64_operand_sum){0};
		}

		sum.is_64bit 
			&= (ops[i].desc.width == W64);
		sum.is_sib_ext
			|= ((ops[i].desc.type & I64_MEM) && (ops[i].desc.type & I64_SIB_EXT));
	}

	sum.is_r0_ext 
		|= (ops[0].desc.type & I64_EXT) && !(ops[0].desc.type & I64_MEM);
	sum.is_r0_ext_mem 
		|= (ops[0].desc.type & I64_EXT) && (ops[0].desc.type & I64_MEM);

	sum.is_r1_ext 
		|= (ops[1].desc.type & I64_EXT) && !(ops[1].desc.type & I64_MEM);
	sum.is_r1_ext_mem 
		|= (ops[1].desc.type & I64_EXT) && (ops[1].desc.type & I64_MEM);

	return sum;
}

#include "i64_emit.h"

axres i64_emit_64(
	_in enum i64_opcode 		opcode,
	_in enum i64_opcode_prefix	prefix,
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

	struct i64_operand_sum sum =
		i64_sum_calc(opcode_desc, ops);
		
	/*
	   	TODO: ADD CASE WHERE 2 OPERAND REGISTERS ARE NOT THE SAME WIDTH
	 	
	 	Check opcode operands and fill [opcode_match] descriptors
	*/

	i64_mte_raw_instr instr = {0};

	/*
		Resolve Legacy prefix for the dummy
	*/




	/*
		Resolve REX for the dummy
	*/

	u8 rex = _i64_rex_resolve(sum);

	unref(prefix);

	*buf = instr;

	__INL_PERF_END
	__INL_PERF_LOG

	io_str(u"REX VALUE:");
	printf("%x\n", rex);

	return AX_SUCC;
}

