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
		 	Determine registers used, 
			and byte length of this instruction
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
		for (u8 i = 0; i < set.op_count; i++){
			ir_operand op = set.ops[i];
			if(op.id != IR_OP_REG){
				continue;
			}

			liveness[op.value] |= blk_shift;
		}

		/*
			Add bytes of the instruction,
			and calculate current block
		*/
		bytes += org_len;
		blk_i = bytes / frag;
	);
/*__INL_PERF_END
__INL_PERF_LOG
	printf("Pass in: %lf\n", (__INL_PERF_SUM / 4.2) / (TBLOCK_SIZE << block->type));*/

	return true;
}
bool tblock_raw_to_ir(
	_in tblock	*block,
	_in_out	u16	org_liveness[0xff]
){
	if (__builtin_expect(block == nullptr, false)){
		return false;
	}
	if (__builtin_expect(org_liveness == nullptr, false)){
		return false;
	}

	// Reference to ir descriptor
	struct ir_context_desc *const desc = 
		&block->ir->desc;

	u16 tar_liveness[0xff] = {0};

	u8 org_len = 0;

	u8 blk_i = 0; // 0-15 index of the block
	u8 next_blk_i = 0; // 0-15 index of the next block

	u32 bytes = 0; // Bytes already passed

	// Fragmentation of each block (Example 32-bytes for [block->type == TBLOCK_BIG])
	const u32 frag = (TBLOCK_SIZE << block->type) / sizeof(ir_raw_instr);

	__TBLOCK_PASS_INIT(block);
	__TBLOCK_PASS_LOOP(org_len,
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
		 	Convert raw instruction to IR
		*/
		ir_raw_instr ir_instr = 
			desc->call.org_to_ir(
				raw_instr,
				block->ir,
				&org_len
			);

		/*
		 	Select tar (host) registers to use
		*/
		for (u8 i = 0; i < ir_instr.set.op_count; i++){
			ir_operand op =
				ir_instr.set.ops[i];
			enum cpu_reg_role op_role = 
				desc->org_map->root[op.value].role;

			// Allocate host register
			u8 tar_id = cpu_alloc_reg(desc->tar_map, op_role);

			// Inherit liveness of that register
			tar_liveness[tar_id] = org_liveness[op.value];
		}

		bytes += org_len;
		next_blk_i = bytes / frag;

		/*
		 	0 if blk is still the same
		 	1 if blk was switched (Invalidate liveness)
		*/
		u8 crossed = next_blk_i - blk_i;
		u16 blk_shift = BIT(next_blk_i);

		/*
		 	Free dead registers after block
		*/
		u8 i = 0;
		u16 bound = (desc->tar_map->reg_count * crossed);
		while(i < bound){
			u8 id = desc->tar_map->root[i].id;
			i++;
			/*
			 	If register is 'alive' in the next block,
				then skip it
			*/
			if (!!(tar_liveness[id] & blk_shift)){
				continue;
			}
			// Free register if liveness does require it
			cpu_free_reg(desc->tar_map, id);
		}
		blk_i = next_blk_i;
	);

	return true;
}
bool tblock_emit(
	_in tblock 	*block
){
	if (__builtin_expect(block == nullptr, false)){
		return false;
	}

	/*
	 	Each u16 bit represents: (For the org (guest))
			((TBLOCK_SIZE << block->type) / sizeof(ir_raw_instr)) IR-instructions.
	*/
	u16 liveness[0xff] = {0};

__INL_PERF_INIT
__INL_PERF_START
	if (__builtin_expect(!tblock_liveness_scan(block, liveness), false)){
		return false;
	}
	if (__builtin_expect(!tblock_raw_to_ir(block, liveness), false)){
		return false;
	}
__INL_PERF_END
__INL_PERF_LOG

	printf("Pass in: %lf\n", (__INL_PERF_SUM / 4.2) / (TBLOCK_SIZE << block->type));

	return true;
}

