#include <ax_type.h>

#include "intel/instr/i64_opcode.h"

/*
 	Quick summary of 
		- operands provided 
		- opcode expected operands
	
	Allows fast and clean way of calculating opcode prefixes
*/
struct i64_operand_sum{
	bool 	is_64bit; // All operands are 64-bit
	bool 	is_sib_ext; // Extended scale base index
	bool 	is_r0_ext; // Register 0 operand extended register
	bool 	is_r0_ext_mem; // Memory 0 operand extended register
	bool 	is_r1_ext; // Register 1 operand extended register
	bool 	is_r1_ext_mem; // Memory 1 operand extended register
};

struct i64_operand_sum i64_sum_calc(
	_in i64_opcode_desc	desc,
	_in i64_operand		ops[I64_RED_OP_COUNT]
);

