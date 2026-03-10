#include "tblock.h"

bool tblock_alloc(
	_in ir_context		*ir,
	_in enum tblock_type 	type,
	_out tblock 		*buf
){
	if (__builtin_expect(ir == nullptr, false)){
		return nullptr;
	}

	*buf = (tblock){
		.type = type,
		.ir = ir,
		.start = ir->desc.code_ptr,
		.end = (u32*)offp(ir->desc.code_ptr, TBLOCK_SIZE << type),
	};
	return true;
}

bool tblock_emit(
	_in tblock 	*block
){
	if (__builtin_expect(block == nullptr, false)){
		return false;
	}

	u16 len = 0;

	/*
	 	Calculate code (guest) memory pointers
	*/
	u32 *code_p0 = block->start;
	u32 *code_p1 = (u32*)offp(block->start, 4);
	u32 *code_p2 = (u32*)offp(block->start, 8);
	u32 *code_p3 = (u32*)offp(block->start, 12);

	// Reference to ir descriptor
	struct ir_context_desc *const desc = 
		&block->ir->desc;

	// Calculate gen (host) memory pointer
	u32 *gen_ptr = desc->gen_ptr;

	// Start loop
	while(code_p0 < block->end){
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
		 	Translate to instruction to IR
		*/
		ir_raw_instr ir_instr = 
			desc->call.org_to_ir(
				raw_instr,
				block->ir
			);

		// Temporary, to avoid compiler optimising
		*gen_ptr = ir_instr.opcode;

		code_p0++;
		gen_ptr++;

		/*
		 	Calculate new offsets
		*/
		code_p1 = (u32*)offp(code_p0, 4);
		code_p2 = (u32*)offp(code_p0, 8);
		code_p3 = (u32*)offp(code_p0, 12);
		len++;
	}
	desc->gen_ptr = gen_ptr;
	desc->code_ptr = code_p0;

	return true;
}

