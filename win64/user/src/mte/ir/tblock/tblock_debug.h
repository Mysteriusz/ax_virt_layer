#include "mte/ir/tblock/tblock.h"

void tblock_liveness_log(
	_in ir_context 	*const ir,
	_in tblock 	*const block
){
	struct ir_context_desc *const desc = &ir->desc;

	for (u16 i = 0; i < desc->guest_map->reg_count; i++){
		u8 id = desc->guest_map->root[i].id;
		io_str(u"Guest liveness for id:");
		io_i64(id);

		printf("\t");
		// Print all bits that assoviate with a fragemntation block
		for (u16 j = 0; j < _tblock_frag_calc(block->type); j++){
			printf("%i", ((block->liveness[id] >> j) & 1));
		}
		printf("\n");
	}
}

void tblock_state_log(
	_in struct cpu_reg_map *map,
	_in tblock 	*const block
){
	for (u16 i = 0; i < map->reg_count; i++){
		u8 id = map->root[i].id;
		io_str(u"Register state for id:");
		io_i64(id);

		printf("\t%i\n", block->state[id]);
	}
}

void tblock_assoc_log(
	_in ir_context 	*const ir,
	_in tblock 	*const block
){
	struct ir_context_desc *const desc = &ir->desc;

	for (u16 i = 0; i < desc->guest_map->reg_count; i++){
		u8 id = desc->guest_map->root[i].id;
		io_str(u"Register association for id:");
		io_i64(id);

		printf("\t%i\n", block->assoc[id].id);
	}
}


