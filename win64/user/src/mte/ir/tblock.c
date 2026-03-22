#include "tblock.h"

bool tblock_alloc(
	_in ir_context		*ir,
	_in enum tblock_type 	type,
	_out tblock 		*buf
){
	if (__builtin_expect(ir == nullptr, false)){
		return nullptr;
	}

	/*
	 	TODO:
		Make some tblock heap buffer and allocate from there
	*/
	*buf = (tblock){
		.type = type,
		.ir = ir,
		.base = ir->desc.code_ptr,
		.ir_count = 0,
		/*
		 	TODO:
			QUICKLY CHANGE TO STATIC BUFFER WHEN IMPLEMENTING TBLOCK TABLE
		*/
		.ir_buf = axmalloc((TBLOCK_SIZE << TBLOCK_BIG) * sizeof(ir_raw_instr)),
	};
	return true;
}

bool tblock_liveness_scan(
	_in tblock	*block,
	_in_out	u16	org_liveness[0xff]
){
	if (__builtin_expect(block == nullptr, false)){
		return false;
	}
	if (__builtin_expect(org_liveness == nullptr, false)){
		return false;
	}

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

			org_liveness[op.value] |= blk_shift;
		}

		/*
			Add bytes of the instruction,
			and calculate current block
		*/
		bytes += org_len;
		blk_i = bytes / frag;
		block->ir_count++;
	);
/*__INL_PERF_END
__INL_PERF_LOG
	printf("Pass in: %lf\n", (__INL_PERF_SUM / 4.2) / (TBLOCK_SIZE << block->type));*/

	return true;
}
bool tblock_raw_to_ir(
	_in tblock		*block,
	_in const u16		org_liveness[0xff], // Liveness table of guest registers (Per instruction block)
	_in tblock_reg_assoc	assoc[0xff] // Guest to host register associations (Per instruction block)
){
	if (__builtin_expect(block == nullptr, false)){
		return false;
	}
	if (__builtin_expect(org_liveness == nullptr, false)){
		return false;
	}
	if (__builtin_expect(assoc == nullptr, false)){
		return false;
	}

	/*
	 	Association table is updated on every block
	*/

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
				*(u32*)code_p0, 
				*(u32*)code_p1, 
				*(u32*)code_p2, 
				*(u32*)code_p3, 
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
			ir_operand op = ir_instr.set.ops[i];
			enum cpu_reg_role op_role = 
				desc->org_map->root[op.value].role;
				
			/*
			 	Check if register doesnt have association
			*/
			if (assoc[op.id].used == false){
				assoc[op.id].used = true;
				assoc[op.id].id = cpu_alloc_role_reg(desc->tar_map, op_role);
			}
		}

		// Save IR instruction to the buffer
		pass_block->ir_buf[pass_i] = ir_instr;

		// Calculate byte offset and block index
		bytes += org_len;
		next_blk_i = bytes / frag;

		/*
		 	0 if blk is still the same
		 	1 if blk was switched (Invalidate liveness)
		*/
		u8 crossed = next_blk_i - blk_i;
		u16 blk_shift = BIT(next_blk_i);

		/*
		 	Free dead registers after block using association table
		*/
		u8 i = 0;
		u16 bound = (desc->org_map->reg_count * crossed);
		while(i < bound){
			u8 org_id = desc->org_map->root[i].id;
			i++;
			/*
			 	If register is 'alive' in the next block,
				then skip it
			*/
			if (!!(org_liveness[org_id] & blk_shift)){
				continue;
			}

			// Free host register since it`s not alive
			assoc[org_id].used = false;
			cpu_free_reg(desc->tar_map, assoc[org_id].id);
		}
		blk_i = next_blk_i;
	);

	return true;
}
bool tblock_ir_to_raw(
	_in tblock		*block
){
	if (__builtin_expect(block == nullptr, false)){
		return false;
	}

	const struct ir_context_desc *desc = &block->ir->desc;

	u8 tar_len = 0;
	u32 ir_i = 0;

	while(ir_i < 2){
		mte_raw_instr tar_instr = 
			desc->call.ir_to_tar(
				block->ir_buf[ir_i],
				block->ir,
				&tar_len);
		ir_i++;
	}

	return true;
}

bool tblock_emit(
	_in tblock 	*block
){
	if (__builtin_expect(block == nullptr, false)){
		return false;
	}

__INL_PERF_INIT
__INL_PERF_START
	/*
	 	Each u16 bit represents a 16-aligned byte block,
		for each register identifier.
			
		IR instruction count for each block is:
			((TBLOCK_SIZE << block->type) / sizeof(ir_raw_instr))
	*/
	u16 org_liveness[0xff] = {0};

	/*
	 	Associations between org and tar registers.
	*/
	tblock_reg_assoc assoc[0xff] = {0};

	if (__builtin_expect(!tblock_liveness_scan(block, org_liveness), false)){
		return false;
	}
	if (__builtin_expect(!tblock_raw_to_ir(block, org_liveness, assoc), false)){
		return false;
	}
	if (__builtin_expect(!tblock_ir_to_raw(block), false)){
		return false;
	}
__INL_PERF_END
__INL_PERF_LOG

	printf("Pass in: %lf\n", (__INL_PERF_SUM / 4.2) / 128);

	return true;
}

