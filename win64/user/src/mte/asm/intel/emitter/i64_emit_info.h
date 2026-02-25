#if !defined(MTE_I64_EMIT_INFO_INT)
#define MTE_I64_EMIT_INFO_INT

#include <ax_type.h>

#include "intel/instr/i64_opcode.h"

/*
 	Quick summary of 
		- operands provided 
		- opcode expected operands
	
	Allows fast and clean way of calculating opcode prefixes
*/
struct i64_operand_sum{
	/*
	 	Width summary

	 	000EDCBA
		E -> Memory addressing with 32-bit register
		D -> All operands are 64-bit
		C -> Reserved
		B -> Reserved
		A -> Any operand is 16-bit
	*/
	u8	width;
	/*
	 	Operand summary
		
	 	000EDCBA
		E -> SIB`s [index] extends a register
		D -> Destination register operand extends a register
		C -> Destination memory operand extends a register
		B -> Source register operand extends a register
		A -> Source memory operand extends a register
	*/
	u8 	operand;
};

struct i64_operand_sum i64_sum_calc(
	_in i64_opcode_desc	desc,
	_in i64_operand		ops[I64_RED_OP_COUNT]
);

#endif // !defined(MTE_I64_EMIT_INFO_INT)

