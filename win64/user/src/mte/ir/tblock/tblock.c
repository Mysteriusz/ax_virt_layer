#include "mte/perf.h"
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

/*
	Fragmentation of each block

	That means that every tblock is
	fragmented into frag-count 'subblocks'

	Example for TBLOCK_SMALL:

		(TBLOCK_SIZE << TBLOCK_SMALL) / sizeof(ir_raw_instr)
		which resolves to:
		(64 << 1) / 16 = 128 / 16 = 8

	Visualisation:
		block{
			frag {16 bytes} -> 8 times
		}
		
	Example for TBLOCK_BIG:

		(TBLOCK_SIZE << TBLOCK_BIG) / sizeof(ir_raw_instr)
		which resolves to:
		(64 << 3) / 16 = 512 / 16 = 16

	Visualisation:
		block{
			frag {16 bytes} -> 16 times
		}
*/
u32 tblock_frag_calc(
	_in enum tblock_type type
){
	return ((TBLOCK_SIZE << type) / sizeof(ir_raw_instr));
}

void tblock_liveness_log(
	_in tblock *block,
	_in u16 liveness[0xff]
){
	struct ir_context_desc *const desc = &block->ir->desc;

	for (u16 i = 0; i < desc->org_map->reg_count; i++){
		u8 id = desc->org_map->root[i].id;
		io_str(u"liveness for id:");
		io_i64(id);
		io_str(u"");
		for (u16 j = 0; j < tblock_frag_calc(block->type); j++){
			io_i64((liveness[id] >> j) & 1);
		}
		io_str(u"");
	}
}

bool tblock_liveness_scan(
	_in_out tblock	*block,
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

	const u32 frag = tblock_frag_calc(block->type);

	/*
	 	Loop through all instructions
		to determine liveness of each register

		If a register is used, it`s automatically marked
		as 'living'  this frag-block (blk_i)

		Visualisation:
			Every instruction will have it`s registers
			analyzed in this example flow

			liveness[reg_0] & BIT(blk_i) = false
			liveness[reg_1] & BIT(blk_i) = false

			add 0, 1 -> reg_0, reg_1

			liveness[reg_0] & BIT(blk_i) = true
			liveness[reg_1] & BIT(blk_i) = true
	*/
	__TBLOCK_PASS_INIT(block);
	__TBLOCK_PASS_LOOP(org_len,
		u16 blk_shift = BIT(blk_i);

		/*
		 	Load instruction with code data
		*/
		mte_raw_instr raw_instr = {
			.arch = desc->org_arch,
		};
		memcpy(raw_instr.payload, code_ptr, 16);
	
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
		 	Loop over all operands in that set
			and set their liveness for the current
			tblock fragment
		*/
		for (u8 i = 0; i < set.ops_count; i++){
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

	return true;
}
bool tblock_raw_to_ir(
	_in tblock			*block,
	_in const u16			org_liveness[0xff], // Calculated liveness table of guest registers (Per instruction block)
	_in_out tblock_reg_assoc	assoc[0xff] // Guest to host register associations (Per instruction block)
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
		};
		memcpy(&raw_instr.payload, code_ptr, 16);

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
		for (u8 i = 0; i < ir_instr.set.ops_count; i++){
			ir_operand op = ir_instr.set.ops[i];
			enum cpu_reg_role op_role = 
				desc->org_map->root[op.value].role;
				
			/*
			 	Check if the operand register doesn`t have an association
			*/
			if (assoc[op.value].used == false){
				assoc[op.value].used = true;
				assoc[op.value].id = cpu_alloc_role_reg(desc->tar_map, op_role);
				io_str(u"Allocated register!");
				io_str(u"Guest id:");
				io_i64(op.value);
				io_str(u"");
				io_str(u"Host id");
				io_i64(assoc[op.value].id);
				io_str(u"");
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
		 	Free dead registers when crossing to
			the next frag-block (blk_i) using association table

			If blk is still the same then crossed == 0,
			which means the loop is ignored
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

	struct ir_context_desc *desc = &block->ir->desc;

	u8 tar_len = 0;
	u32 ir_i = 0;

	while(ir_i < 2){
		mte_raw_instr tar_instr = 
			desc->call.ir_to_tar(
				block->ir_buf[ir_i],
				block->ir,
				&tar_len);

		/*
			TODO!!!

			This is only temporary and should be removed due to the overhead
			Maybe use SIMD?
		*/
		memcpy(desc->gen_ptr, tar_instr.payload, tar_len);

		desc->gen_ptr = offp(desc->gen_ptr, tar_len);
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
	 	Each bit of the u16 represents the 16-byte aligned block,
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

	printf("Average translation in: %lfns\n", (__INL_PERF_SUM / 4.2) / 256);

	return true;
}

