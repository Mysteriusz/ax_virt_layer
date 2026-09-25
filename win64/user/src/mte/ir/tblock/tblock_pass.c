#include "asm/asm_types.h"
#include "asm/asm_instr_fix.h"

#include "mte/ir/tblock/tblock.h"
#include "mte/ir/tblock/tblock_pass.h"
#include "mte/ir/tblock/tblock_debug.h"

struct tblock_pass_result tblock_liveness_scan(
	_in ir_context	*const ir,
	_in_out tblock	*const block
){
	if (__builtin_expect(ir == nullptr, false)){
		return (struct tblock_pass_result){.res = AX_INV_ARG};
	}
	if (__builtin_expect(block == nullptr, false)){
		return (struct tblock_pass_result){.res = AX_INV_ARG};
	}

	u8 guest_len = 0; // Byte length of the instruction itself
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
	__TBLOCK_PASS_LOOP(guest_len,
		u16 blk_shift = BIT(blk_i);

		/*
		 	Load instruction with code data
		*/
		mte_raw_instr raw_instr = {
			.arch = _TBLOCK_DESC->guest_arch,
		};
		memcpy(raw_instr.payload, _TBLOCK_GUEST_PTR, 16);
	
		/*
		 	Determine registers used, 
			and byte length of this instruction
		*/
		ir_operand_set set = 
			_TBLOCK_DESC->call.guest_reg_fetch(
				raw_instr, ir,
				&guest_len
			);

		/*
		 	Loop over all operands in that set
			and set their liveness for the current frag-block
		*/
		for (u8 i = 0; i < set.ops_count; i++){
			ir_operand op = set.ops[i];
			if(op.kind != IR_OP_REG){
				continue;
			}

			_TBLOCK->liveness[op.id] |= blk_shift;
		}

		/*
			Add bytes of the instruction,
			and calculate current block
		*/
		bytes += guest_len;
		blk_i = bytes / _TBLOCK_FRAG;
	);

	return (struct tblock_pass_result){.count = _TBLOCK_PASS_IDX, .res = AX_SUCC};
}
struct tblock_pass_result tblock_raw_to_ir(
	_in ir_context	*const ir,
	_in_out tblock	*const block
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

	u8 guest_len = 0;

	u8 blk_i = 0; // 0-15 index of the block
	u8 next_blk_i = 0; // 0-15 index of the next block

	u32 bytes = 0; // Bytes already passed

	__TBLOCK_PASS_INIT(ir, block);
	unref(_TBLOCK_PASS_IDX);

	__TBLOCK_PASS_LOOP(guest_len,
		/*
		 	Load instruction with code data
		*/
		mte_raw_instr raw_instr = {
			.arch = _TBLOCK_DESC->guest_arch,
		};
		memcpy(raw_instr.payload, _TBLOCK_GUEST_PTR, 16);

		/*
		 	Convert guest instruction to IR
		*/
		ir_raw_instr ir_instr = 
			_TBLOCK_DESC->call.guest_to_ir(
				raw_instr, ir,
				&guest_len);
		if (__builtin_expect(ir_instr.opcode == IR_INVALID_OPCODE, false)){
			goto skip;
		}

		/*
		 	Fill associations for the current state
			(Allocate registers for the host cpu)
		*/
		asm_fill_assoc(ir, &ir_instr, 
			&_TBLOCK->assoc,
			&_TBLOCK->state);

		/*
		 	Fix the instruction given association
		*/
		asm_fix_instr(ir, &ir_instr,
			&_TBLOCK->assoc);


		/*
		 	Expand and save the IR instruction to the IR buffer
		*/
		u32 expanded = asm_expand_instr(ir, &ir_instr,
			_TBLOCK->ir_buf.capacity - _TBLOCK->ir_cnt,
			&_TBLOCK->ir_buf.base[_TBLOCK->ir_cnt]);
		_TBLOCK->ir_cnt += expanded;

skip: // TEMP
		// Calculate byte offset and block index
		bytes += guest_len;
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
			asm_flush_by_liveness(ir, blk_i,
				&_TBLOCK->liveness,
				&_TBLOCK->assoc,
				&_TBLOCK->state);
			blk_i = next_blk_i;
		}
	);

	return (struct tblock_pass_result){.count = _TBLOCK->ir_cnt, .res = AX_SUCC};
}

struct tblock_pass_result tblock_ir_to_raw(
	_in_out ir_context	*const ir,
	_in tblock		*const block
){
	if (__builtin_expect(ir == nullptr, false)){
		return (struct tblock_pass_result){.res = AX_INV_ARG};
	}
	if (__builtin_expect(block == nullptr, false)){
		return (struct tblock_pass_result){.res = AX_INV_ARG};
	}

	struct ir_context_desc *const desc = &ir->desc;

	u8 host_len = 0;
	u32 ir_i = 0;

	while(ir_i < block->ir_cnt){
		auto ir_instr = (ir_raw_instr *const)&block->ir_buf.base[ir_i];
		asrt(ir_instr->opcode != IR_INVALID_OPCODE,
			io_str(u"Invalid IR opcode generated at index");
			io_i64(ir_i);
			io_str(u"Out of:");
			io_i64(block->ir_cnt);
			printf("Opcode: %u\n", ir_instr->opcode);
		);

		mte_raw_instr host_instr = 
			desc->call.ir_to_host(
				*ir_instr,
				ir,
				&host_len);

		/*
			TODO!!!

			This is only temporary and should be removed due to the overhead
			Maybe use SIMD?
		*/
		memcpy(desc->host_ptr, host_instr.payload, host_len);

		desc->host_ptr = offp(desc->host_ptr, host_len);
		ir_i++;
	}

	return (struct tblock_pass_result){.count = block->ir_cnt, .res = AX_SUCC};
}

