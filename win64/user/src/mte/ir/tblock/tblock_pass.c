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
		for (u16 j = 0; j < tblock_frag_calc(block->type); j++){
			printf("%i", ((liveness[id] >> j) & 1));
		}
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
	//u32 ir_idx = 0; // Index of 'tblock->ir_buf' buffer

	__TBLOCK_PASS_INIT(ir, block);
	__TBLOCK_PASS_LOOP(org_len,
		/*
		 	Load instruction with code data
		*/
		mte_raw_instr raw_instr = {
			.arch = _TBLOCK_DESC->org_arch,
		};
		memcpy(raw_instr.payload, _TBLOCK_CODE_PTR, 16);

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

		/*
		 	Expand and save the IR instruction to the IR buffer
		*/
		_TBLOCK_PASS_BLOCK->ir_len += comp_expand_instr(ir, &ir_instr,
			_TBLOCK_PASS_BLOCK->ir_buf
		);

skip: // TEMP
		// Calculate byte offset and block index
		bytes += org_len;
		next_blk_i = bytes / _TBLOCK_FRAG;

		/*
		 	0 if blk is still the same
		 	1 if blk was switched (Invalidate liveness)
		*/
		u8 crossed = !!(next_blk_i - blk_i);

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
			blk_i = next_blk_i;
		}
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

	while(ir_i < block->ir_len){
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

	return (struct tblock_pass_result){.count = block->ir_len, .res = AX_SUCC};
}

