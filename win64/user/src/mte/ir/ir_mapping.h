#if !defined(MTE_IR_MAPPING_INT)
#define MTE_IR_MAPPING_INT

#include <ax_type.h>

#include "ir.h"

#define IR_INSTR_MAP_SPECIAL 0xffff
struct ir_instr_map{
	_in struct ir_context_desc	*desc;
	/*
	   	[org_map] = desc->org_map
	   	[tar_map] = desc->tar_map

	   	Map of [org_map] index to [tar_map] index

		OOOOOOOOTTTTTTTT

		O -> 8-bit [org_map] index
		T -> 8-bit [tar_map] index

	*/
	u16 map[4];
	u8 count; // Count of entries in [map]
};

// TODO: After implementing translation blocks
bool ir_create_tblock_mapping(
);

/*
 	Single instruction architecture mapping
*/
bool ir_create_instr_mapping(
	_in struct ir_context_desc	*desc,
	/*
	 	Instruction translated via [ir_context_desc->call.org_to_ir]
	*/
	_in ir_raw_instr 		instr,
	_out struct ir_instr_map	*map
);

#endif // !defined(MTE_IR_MAPPING_INT)

