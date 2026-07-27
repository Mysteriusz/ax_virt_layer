#include "compile_alloc.h"
#include "compile_op_fix.h"

void comp_fill_assoc(
	_in const ir_context 	*ir,
	_in ir_raw_instr 	*ir_instr,
	_in_out comp_reg_assoc	(*assoc)[0xff + IR_SPILL_LIMIT],
	_in_out comp_reg_state	(*state)[0xff + IR_SPILL_LIMIT]
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
		u16 alloc = comp_alloc_reg(tar_map, org_role, state);

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

void comp_fix_instr(
	_in const ir_context 	*ir,
	_in_out ir_raw_instr 	*ir_instr,
	_in_out comp_reg_assoc	(*assoc)[0xff + IR_SPILL_LIMIT],
	_in_out comp_reg_state	(*state)[0xff + IR_SPILL_LIMIT]
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
}

void comp_flush_by_liveness(
	_in const ir_context 		*ir,
	_in const u8			liveness_block_idx,
	_in const comp_reg_liveness	(*liveness)[0xff],
	_in_out comp_reg_assoc		(*assoc)[0xff + IR_SPILL_LIMIT],
	_in_out comp_reg_state		(*state)[0xff + IR_SPILL_LIMIT]
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

