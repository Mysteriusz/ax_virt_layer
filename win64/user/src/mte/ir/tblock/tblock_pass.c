#include "compile/compile_types.h"
#include "compile/compile_op_fix.h"

#include "tblock.h"
#include "tblock_pass.h"

void tblock_liveness_log(
	_in ir_context *ir,
	_in tblock *block,
	_in u16 liveness[0xff]
){
	struct ir_context_desc *const desc = &ir->desc;

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

struct tblock_pass_result tblock_liveness_scan(
	_in ir_context	*ir,
	_in_out tblock	*block
){
	if (__builtin_expect(ir == nullptr, false)){
		return (struct tblock_pass_result){.res = AX_INV_ARG};
	}
	if (__builtin_expect(block == nullptr, false)){
		return (struct tblock_pass_result){.res = AX_INV_ARG};
	}

	u8 org_len = 0; // Byte length of the instruction itself
	u8 blk_i = 0; // 0-15 index of the block
	u32 bytes = 0; // Bytes already passed

	__TBLOCK_PASS_INIT(ir, block);
	unref(_TBLOCK_PASS_IDX);
		
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
	__TBLOCK_PASS_LOOP(org_len,
		u16 blk_shift = BIT(blk_i);

		/*
		 	Load instruction with code data
		*/
		mte_raw_instr raw_instr = {
			.arch = _TBLOCK_DESC->org_arch,
		};
		memcpy(raw_instr.payload, _TBLOCK_CODE_PTR, 16);
	
		/*
		 	Determine registers used, 
			and byte length of this instruction
		*/
		ir_operand_set set = 
			_TBLOCK_DESC->call.org_reg_fetch(
				raw_instr,
				ir,
				&org_len
			);

		/*
		 	Loop over all operands in that set
			and set their liveness for the current frag-block
		*/
		for (u8 i = 0; i < set.ops_count; i++){
			ir_operand op = set.ops[i];
			if(op.id != IR_OP_REG){
				continue;
			}

			_TBLOCK_PASS_BLOCK->liveness[op.id] |= blk_shift;
		}

		/*
			Add bytes of the instruction,
			and calculate current block
		*/
		bytes += org_len;
		blk_i = bytes / _TBLOCK_FRAG;

		_TBLOCK_PASS_BLOCK->ir_len++;
	);

	return (struct tblock_pass_result){.count = _TBLOCK_PASS_IDX, .res = AX_SUCC};
}
struct tblock_pass_result tblock_raw_to_ir(
	_in ir_context	*ir,
	_in_out tblock	*block
){
	if (__builtin_expect(ir == nullptr, false)){
		return (struct tblock_pass_result){.res = AX_INV_ARG};
	}
	if (__builtin_expect(block == nullptr, false)){
		return (struct tblock_pass_result){.res = AX_INV_ARG};
	}

	/*
	 	Association table is updated on every block
	*/

	u8 org_len = 0;

	u8 blk_i = 0; // 0-15 index of the block
	u8 next_blk_i = 0; // 0-15 index of the next block

	u32 bytes = 0; // Bytes already passed

	__TBLOCK_PASS_INIT(ir, block);
	__TBLOCK_PASS_LOOP(org_len,
		/*
		 	Load instruction with code data
		*/
		mte_raw_instr raw_instr = {
			.arch = _TBLOCK_DESC->org_arch,
		};
		memcpy(&raw_instr.payload, _TBLOCK_CODE_PTR, 16);

		/*
		 	Convert org (guest) instruction to IR
		*/
		ir_raw_instr ir_instr = _TBLOCK_DESC->call.org_to_ir(
				raw_instr,
				ir,
				&org_len
			);
		if (ir_instr.opcode == IR_INVALID_OPCODE){
			goto skip;
		}

		/*
		 	Fill associations for the current state
			(Allocate registers for the tar (Host) cpu)
		*/
		comp_fill_assoc(ir, &ir_instr, 
			&_TBLOCK_PASS_BLOCK->assoc,
			&_TBLOCK_PASS_BLOCK->state);

		/*
		 	Fix the instruction
			given association and state
		*/
		comp_fix_instr(ir, &ir_instr,
			&_TBLOCK_PASS_BLOCK->assoc,
			&_TBLOCK_PASS_BLOCK->state);
#if 0
		for (u8 i = 0; i < ir_instr.set.ops_count; i++){
			ir_operand *ir_op = &ir_instr.set.ops[i];

			switch(ir_op->id){
			case IR_OP_REG:
				struct cpu_reg_desc *op_desc = 
					&_TBLOCK_DESC->org_map->root[ir_op->value];
				comp_reg_assoc *assoc =
					&_TBLOCK_PASS_BLOCK->assoc[ir_op->value];

				/*
			 		Check if the operand register doesn`t have an association
				*/
				if (assoc->used == false){
					u16 reg_id = tblock_alloc_reg(
						&_TBLOCK_PASS_BLOCK->state,
						_TBLOCK_DESC->tar_map,
						op_desc->role);
					
					u8 tar_id = (reg_id & (reg_id >> 8)) & 0xff;
					u8 spill = (reg_id & 0xff) == 0xff;

					assoc->used = true;
					assoc->spill = spill;

					/*
					 	Repurpose the IR operand to read/write spill memory
					*/
					if (spill){
						io_str(u"SPILLED");
						tar_id = _ir_sbuf_inc(&ir->desc.ptr_buf);
						ir_op->id = IR_OP_MEM;
					}

					io_str(u"Allocated register/spill!");
					io_str(u"Guest id:");
					io_i64(ir_op->value);
					io_str(u"");
					io_str(u"Host id");
					io_i64(tar_id);
					io_str(u"");

					assoc->id = tar_id;
					ir_op->value = tar_id;
				}else{
					ir_op->value = assoc->id;
				}

				/*
					Convert operand values to what tar (host) accepts
				*/
			default:
				continue;
			}
		}
#endif

		// Save IR instruction to the IR buffer
		_TBLOCK_PASS_BLOCK->ir_buf[_TBLOCK_PASS_IDX] = ir_instr;

skip:
		// Calculate byte offset and block index
		bytes += org_len;
		next_blk_i = bytes / _TBLOCK_FRAG;

		/*
		 	0 if blk is still the same
		 	1 if blk was switched (Invalidate liveness)
		*/
		u8 crossed = next_blk_i - blk_i;
		u16 blk_shift = BIT(next_blk_i);
		unref(crossed);
		unref(blk_shift);

		/*
		 	Free dead registers when crossing to
			the next frag-block (blk_i) using association table

			If blk is still the same then crossed == 0,
			which means the loop is ignored
		*/
		if (crossed){
			comp_flush_by_liveness(ir, blk_i,
				&_TBLOCK_PASS_BLOCK->liveness,
				&_TBLOCK_PASS_BLOCK->assoc,
				&_TBLOCK_PASS_BLOCK->state);
		}
#if 0
		u8 i = 0;
		u16 bound = (_TBLOCK_DESC->org_map->reg_count * crossed);
		while(i < bound){
			u8 org_id = _TBLOCK_DESC->org_map->root[i].id;
			i++;
			/*
			 	If register is 'alive' in the next block,
				then skip it
			*/
			if (!!(_TBLOCK_PASS_BLOCK->liveness[org_id] & blk_shift)){
				continue;
			}

			// Free host register since it`s not alive
			tblock_free_reg(&_TBLOCK_PASS_BLOCK->state,
				_TBLOCK_PASS_BLOCK->assoc[org_id].id);

			_TBLOCK_PASS_BLOCK->assoc[org_id].used = false;
			_TBLOCK_PASS_BLOCK->assoc[org_id].spill = false;
			_TBLOCK_PASS_BLOCK->assoc[org_id].id = 0;
		}
		blk_i = next_blk_i;
#endif
	);

	return (struct tblock_pass_result){.count = _TBLOCK_PASS_IDX, .res = AX_SUCC};
}

struct tblock_pass_result tblock_ir_to_raw(
	_in_out ir_context	*ir,
	_in tblock		*block
){
	if (__builtin_expect(ir == nullptr, false)){
		return (struct tblock_pass_result){.res = AX_INV_ARG};
	}
	if (__builtin_expect(block == nullptr, false)){
		return (struct tblock_pass_result){.res = AX_INV_ARG};
	}

	struct ir_context_desc *desc = &ir->desc;

	u8 tar_len = 0;
	u32 ir_i = 0;
	u32 run = 2;

	while(ir_i < run){
		ir_raw_instr ir_instr = block->ir_buf[ir_i];
		mte_raw_instr tar_instr = 
			desc->call.ir_to_tar(
				ir_instr,
				ir,
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

	return (struct tblock_pass_result){.count = run, .res = AX_SUCC};
}

