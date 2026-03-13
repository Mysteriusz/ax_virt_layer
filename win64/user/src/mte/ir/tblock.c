#include "tblock.h"

bool tblock_alloc(
	_in ir_context		*ir,
	_in enum tblock_type 	type,
	_out tblock 		*buf
){
	if (__builtin_expect(ir == nullptr, false)){
		return nullptr;
	}

	*buf = (tblock){
		.type = type,
		.ir = ir,
		.base = ir->desc.code_ptr,
	};
	return true;
}

bool tblock_liveness_scan(
	_in tblock	*block,
	_in_out	u16	liveness[0xff]
){
	// Reference to ir descriptor
	struct ir_context_desc *const desc = 
		&block->ir->desc;

	u8 org_len = 0; // Byte length of the instruction itself

	u8 buf_i = 0; // Index of the current operand
	u8 blk_i = 0; // 0-15 index of the block

	u32 bytes = 0; // Bytes already passed

	// Fragmentation of each block (Example 32-bytes for [block->type == TBLOCK_BIG])
	const u32 frag = (TBLOCK_SIZE << block->type) / sizeof(ir_raw_instr);

/*__INL_PERF_INIT
__INL_PERF_START*/
	__TBLOCK_PASS_INIT(block);
	__TBLOCK_PASS_LOOP(org_len,
		u16 blk_shift = BIT(blk_i);

		/*
		 	Load instruction with code data
		*/
		mte_raw_instr raw_instr = {
			.arch = desc->org_arch,
			.payload = {
				*code_p0, 
				*code_p1, 
				*code_p2, 
				*code_p3, 
			},
		};
	
		/*
		 	Translate to instruction to IR
		*/
		ir_operand_set set = 
			desc->call.org_reg_fetch(
				raw_instr,
				block->ir,
				&org_len
			);

		/*
		 	Loop over all operands in that set,
			and write them to buf_map
		*/
		for (buf_i = 0; buf_i < set.op_count; buf_i++){
			ir_operand op = set.ops[buf_i];
			if(op.id != IR_OP_REG){
				continue;
			}

			liveness[op.value] |= blk_shift;
		}
		liveness[set.ret.value] |= blk_shift;

		blk_i = bytes / frag;
		bytes += org_len;
	);
/*__INL_PERF_END
__INL_PERF_LOG
	printf("Pass in: %lf\n", (__INL_PERF_SUM / 4.2) / (TBLOCK_SIZE << block->type));*/

	return true;
}
bool tblock_emit(
	_in tblock 	*block
){
	if (__builtin_expect(block == nullptr, false)){
		return false;
	}

	/*
	 	Each bit represents:
			((TBLOCK_SIZE << block->type) / sizeof(ir_raw_instr)) IR-instructions.
	*/
	u16 liveness[0xff] = {0};

__INL_PERF_INIT
__INL_PERF_START
	volatile bool res = tblock_liveness_scan(block, liveness);
__INL_PERF_END
__INL_PERF_LOG

	printf("Pass in: %lf\n", (__INL_PERF_SUM / 4.2) / (TBLOCK_SIZE << block->type));
	io_i64(liveness[11]);
	io_i64(liveness[10]);
	io_i64(liveness[9]);

	return true;
}

