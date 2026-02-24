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
 	/*
	 	Scale base index used [reg+base*index]
		Example:
			- (r8+r9*2)

		For cases without [base] as extended register this should be false
		(I64_SIB flag should not be provided in the ops)
	*/
	bool 	is_sib_ext;
	bool 	is_r0_ext; // Register 0 operand extended register
	bool 	is_r0_ext_mem; // Memory 0 operand extended register
	bool 	is_r1_ext; // Register 1 operand extended register
	bool 	is_r1_ext_mem; // Memory 1 operand extended register
};

struct i64_operand_sum i64_sum_calc(
	_in i64_opcode_desc	desc,
	_in i64_operand		ops[I64_RED_OP_COUNT]
);

