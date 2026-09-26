#include "mte/ir/tblock/tblock.h"

_inline_avert static void _tblock_liveness_log(
	_in ir_context 	*const ir,
	_in tblock 	*const block
){
	struct ir_context_desc *const desc = &ir->desc;

	for (u16 i = 0; i < desc->guest_map->reg_count; i++){
		u8 id = desc->guest_map->root[i].id;
		io_afstr(ANSI("Guest liveness for id: %lli\n"), id);

		printf("\t");
		// Print all bits that assoviate with a fragemntation block
		for (u16 j = 0; j < _tblock_frag_calc(block->type); j++){
			printf("%i", ((block->liveness[id] >> j) & 1));
		}
		printf("\n");
	}
}

_inline_avert static void _tblock_state_log(
	_in const struct cpu_reg_map 	*const map,
	_in tblock 			*const block
){
	for (u16 i = 0; i < map->reg_count; i++){
		u8 id = map->root[i].id;
		io_afstr(ANSI("Register state for id: %lli\n"), id);
		io_afstr(ANSI("\t%i\n"), block->state[id]);
	}
}

_inline_avert static void _tblock_assoc_log(
	_in ir_context 	*const ir,
	_in tblock 	*const block
){
	struct ir_context_desc *const desc = &ir->desc;

	for (u16 i = 0; i < desc->host_map->reg_count; i++){
		u8 id = desc->host_map->root[i].id;
		io_afstr(ANSI("Register association for id: %lli\n"), id);
		io_afstr(ANSI("\t%i\n"), block->assoc[id].id);
	}
}

_inline_avert static void _tblock_error_log(
	_in ir_context 	*const ir,
	_in tblock 	*const block
){
	io_astr(ANSI("Guest to Host liveness log:\n"));
	_tblock_liveness_log(ir, block);
	io_astr(ANSI("Guest state log:\n"));
	_tblock_state_log(ir->desc.guest_map, block);
	io_astr(ANSI("Host state log:\n"));
	_tblock_state_log(ir->desc.host_map, block);
	io_astr(ANSI("Host association log:\n"));
	_tblock_assoc_log(ir, block);
}

