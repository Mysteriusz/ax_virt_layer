#include "asm_alloc.h"
#include "asm_instr_fix.h"

void asm_fill_assoc(
	_in const ir_context 	*ir,
	_in ir_raw_instr 	*ir_instr,
	_in_out asm_reg_assoc	(*assoc)[0xff + IR_SPILL_LIMIT],
	_in_out asm_reg_state	(*state)[0xff + IR_SPILL_LIMIT]
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

	struct cpu_reg_map *org_map = ir->desc.org_map;
	struct cpu_reg_map *tar_map = ir->desc.tar_map;

	/*
	 	Iterate over the entire instruction operand set
		and process each operand individually
	*/
	for (u8 i = 0; i < ir_instr->set.ops_count; i++){
		ir_operand *ir_reg = &ir_instr->set.ops[i];

		/*
			TODO: Reconsider what exactly to do in that case
		*/
		if ((*assoc)[ir_reg->id].used == true){
			continue;
		}

		// Role of the org (Guest) register
		enum cpu_reg_role org_role = org_map->root[ir_reg->id].role;

		// Allocate tar (Host) the register/spill
		u16 alloc = asm_alloc_reg(tar_map, org_role, state);

		/*
			Calculate the effective id and spill 
			based on the 'asm_alloc_reg' return value encoding
		*/
		u8 alloc_id = (alloc & (alloc >> 8)) & 0xff;
		bool spill = alloc >> 8 != 0xff;

		/*
			TODO: Support spilling
		*/
		asrt(!spill, ax_log_msg(AX_NOT_IMP,
			u"Register spilling not supported."));

		(*assoc)[ir_reg->id].spill = spill;
		(*assoc)[ir_reg->id].used = true;
		(*assoc)[ir_reg->id].id = alloc_id;
	}
}

void asm_fix_instr(
	_in const ir_context 	*ir,
	_in_out ir_raw_instr 	*ir_instr,
	_in_out asm_reg_assoc	(*assoc)[0xff + IR_SPILL_LIMIT],
	_in_out asm_reg_state	(*state)[0xff + IR_SPILL_LIMIT]
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

	for (u8 i = 0; i < ir_instr->set.ops_count; i++){
		// Load org (Guest) register id that`s currently stored in the IR
		u8 org_id = ir_instr->set.ops[i].id;

		// Load tar (Host) register id that`s associated to 'org_id'
		u8 tar_id = (*assoc)[org_id].id;

		// Swap the org (Guest) register id to tar (Host) register id
		ir_instr->set.ops[i].id = tar_id;
	}
}

u32 asm_expand_instr(
	_in const ir_context 		*ir,
	_in const ir_raw_instr 		*restrict ir_instr,
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

	if (__builtin_expect(ir_instr == buf, false)){
		asrt(0, ax_log_msg(AX_INV_ARG,
			u"Instruction pointer cannot be equal to the instruction buffer"));
	}

	u8 org_isa = MTE_ARCH_ISA_FORM(ir->desc.org_arch);
	u8 tar_isa = MTE_ARCH_ISA_FORM(ir->desc.tar_arch);

	u8 count = 0;

	/*
	 	Best case scenario that ISA forms are the same
		and there is no need for expansion
	*/
	if (org_isa == tar_isa){
		buf[0] = *ir_instr;
		return 1;
	}

	const ir_operand_set *set = &ir_instr->set;

	switch(tar_isa){
	case MTE_ISA_TWO_OP:
		/*
		 	Expansion required since either dest/src is not present
			or the opcode doesn`t require non dest/src operands
		*/

		if (!(set->ops[0].id == set->ops[1].id || ir_instr->opcode & IR_DEST_SRC_ACC)){
			/*
			 	Create a MOV instruction based on the current opcode

				r0 = r1
			*/
			ir_raw_instr mov_instr = (ir_raw_instr){
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
	_in const asm_reg_liveness	(*liveness)[0xff],
	_in_out asm_reg_assoc		(*assoc)[0xff + IR_SPILL_LIMIT],
	_in_out asm_reg_state		(*state)[0xff + IR_SPILL_LIMIT]
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
	struct cpu_reg_map *org_map = ir->desc.org_map; 

	/*
	 	Iterate over all registers in the assoc and clear them
	*/
	for(u8 i = 0; i < org_map->reg_count; i++){
		bool alive = (*liveness)[i] & block_bit;
		if (alive){
			continue;
		}

		u8 reg_id = org_map->root[i].id;

		/*
		 	Cleanup the register assoc entry
		*/
		(*assoc)[reg_id].id = 0;
		(*assoc)[reg_id].used = false;
		(*assoc)[reg_id].spill = false;

		/*
		 	Cleanup the register state entry
		*/
		(*state)[reg_id] = REG_FREE;
	}
}

