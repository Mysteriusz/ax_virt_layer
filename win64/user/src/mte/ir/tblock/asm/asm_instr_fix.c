#include "asm_alloc.h"
#include "asm_instr_fix.h"

void asm_fill_assoc(
	_in const ir_context 	*ir,
	_in ir_raw_instr 	*ir_instr,
	_in_out asm_reg_assoc	(*assoc)[IR_SPILL_REG_LIMIT],
	_in_out asm_reg_state	(*state)[IR_SPILL_REG_LIMIT]
){
	if (__builtin_expect(ir == nullptr, false)){
		return;
	}
	if (__builtin_expect(ir_instr == nullptr, false)){
		return;
	}
	if (__builtin_expect(assoc == nullptr, false)){
		return;
	}
	if (__builtin_expect(state == nullptr, false)){
		return;
	}

	u8 idx = ir_instr->set.ops[0].id == ir_instr->set.ops[1].id
		? 1 : 0;
	struct cpu_reg_map *guest_map = ir->desc.guest_map;
	struct cpu_reg_map *host_map = ir->desc.host_map;

	/*
	 	Iterate over the entire instruction operand set
		and process each operand individually
	*/
	for (u8 i = idx; i < ir_instr->set.ops_count; i++){
		ir_operand *ir_reg = &ir_instr->set.ops[i];

		/*
		 	Reuse the register
		*/
		if (_ir_reg_assoc_used((*assoc)[ir_reg->id])){
			return;
		}

		// Role of the guest (Guest) register
		enum cpu_reg_role guest_role = guest_map->root[ir_reg->id].role;

		// Allocate host (Host) the register/spill
		u16 alloc = asm_alloc_reg(host_map, guest_role, state);

		/*
			Calculate the effective id and spill 
			based on the 'asm_alloc_reg' return value encoding
		*/
		u8 alloc_id = (alloc & (alloc >> 8)) & 0xff;
		bool is_spill = alloc >> 8 != 0xff;

		/*
			TODO: Support spilling
		*/
		asrt(!is_spill, ax_log_msg(AX_NOT_IMP,
			u"Register spilling not supported."));

		(*assoc)[ir_reg->id].flags = _ir_reg_assoc_flags(true, is_spill);
		(*assoc)[ir_reg->id].id = alloc_id;
	}
}

void asm_fix_instr(
	_in const ir_context 	*ir,
	_in_out ir_raw_instr 	*ir_instr,
	_in_out asm_reg_assoc	(*assoc)[IR_SPILL_REG_LIMIT]
){
	if (__builtin_expect(ir == nullptr, false)){
		return;
	}
	if (__builtin_expect(ir_instr == nullptr, false)){
		return;
	}
	if (__builtin_expect(assoc == nullptr, false)){
		return;
	}

	for (u8 i = 0; i < ir_instr->set.ops_count; i++){
		// Load guest (Guest) register id that`s currently stored in the IR
		u8 guest_id = ir_instr->set.ops[i].id;

		// Load host (Host) register id that`s associated to 'guest_id'
		u8 host_id = (*assoc)[guest_id].id;

		// Swap the guest (Guest) register id to host (Host) register id
		ir_instr->set.ops[i].id = host_id;
	}
}

u32 asm_expand_instr(
	_in const ir_context 		*ir,
	_in const ir_raw_instr 		*restrict ir_instr,
	_in const u32			buf_len,
	_in_out ir_raw_instr 		*restrict buf
){
	if (__builtin_expect(ir == nullptr, false)){
		return 0;
	}
	if (__builtin_expect(ir_instr == nullptr, false)){
		return 0;
	}
	if (__builtin_expect(buf == nullptr, false)){
		return 0;
	}
	if (__builtin_expect(buf_len == 0, false)){
		return 0;
	}

	if (__builtin_expect(ir_instr == buf, false)){
		asrt(0, ax_log_msg(AX_INV_ARG,
			u"Instruction pointer cannot be equal to the instruction buffer"));
	}

	u8 guest_isa = MTE_ARCH_ISA_FORM(ir->desc.guest_arch);
	u8 host_isa = MTE_ARCH_ISA_FORM(ir->desc.host_arch);

	u8 count = 0;

	/*
	 	Best case scenario that ISA forms are the same
		and there is no need for expansion
	*/
	if (guest_isa == host_isa){
		buf[0] = *ir_instr;
		return 1;
	}

	const ir_operand_set *set = &ir_instr->set;

	switch(host_isa){
	case MTE_ISA_TWO_OP:
		if (__builtin_expect(buf_len <= 1, false)){
			asrt(0, ax_log_msg(AX_BUF_TOO_SMALL,
				u"Not enough space left in the buffer for instruction fix."));
		}

		/*
		 	Expansion required since either dest/src is not present
			or the opcode doesn`t require non dest/src operands
		*/

		if (set->ops[0].id != set->ops[1].id && !(IR_OPCODE_FLAGS(ir_instr->opcode) & IR_DEST_NEQ_SRC)){
			/*
			 	Create a MOV instruction based on the current opcode

				r0 = r1
			*/
			ir_raw_instr mov_instr = (ir_raw_instr){
				/*
					Only swap the group since we want to keep
					the bit-width of the moved data
				*/
				.opcode = ir_opcode_swap_group(ir_instr->opcode, IR_GROUP_MOV),
				.set = (ir_operand_set){
					.ops[0] = set->ops[0],
					.ops[1] = set->ops[1],
					.ops_count = 2,
				}, 
			};
			buf[0] = mov_instr;
			count = 1;
		}

		/*
		 	Create a new folded instruction 

			r0 = r0, r2
		*/
		ir_raw_instr fix_instr = (ir_raw_instr){
			.opcode = ir_instr->opcode,
			.set = (ir_operand_set){
				.ops[0] = set->ops[0],
				.ops[1] = set->ops[0],
				.ops[2] = set->ops[2],
				.ops[3] = set->ops[3],
				.ops[4] = set->ops[4],
				.ops[5] = set->ops[5],
				.ops_count = set->ops_count,
			}, 
		};

		buf[count] = fix_instr;
		count++;

		break;
	default:
		asrt(0, ax_log_msg(AX_NOT_IMP,
			u"This ISA form expansion is not supported"));
	}

	return count;
}

void asm_flush_by_liveness(
	_in const ir_context 		*ir,
	_in const u8			liveness_block_idx,
	_in const asm_reg_liveness	(*liveness)[IR_REG_LIMIT],
	_in_out asm_reg_assoc		(*assoc)[IR_SPILL_REG_LIMIT],
	_in_out asm_reg_state		(*state)[IR_SPILL_REG_LIMIT]
){
	if (__builtin_expect(ir == nullptr, false)){
		return;
	}
	if (__builtin_expect(liveness == nullptr, false)){
		return;
	}
	if (__builtin_expect(assoc == nullptr, false)){
		return;
	}
	if (__builtin_expect(state == nullptr, false)){
		return;
	}

	const u16 block_bit = BIT(liveness_block_idx);
	struct cpu_reg_map *guest_map = ir->desc.guest_map; 

	/*
	 	Iterate over all registers in the assoc and clear them
	*/
	for(u8 i = 0; i < guest_map->reg_count; i++){
		bool alive = (*liveness)[i] & block_bit;
		if (alive){
			continue;
		}

		u8 reg_id = guest_map->root[i].id;

		/*
		 	Cleanup the register assoc entry
		*/
		(*assoc)[reg_id].id = 0;
		(*assoc)[reg_id].flags = 0;

		/*
		 	Cleanup the register state entry
		*/
		(*state)[reg_id] = REG_FREE;
	}
}

