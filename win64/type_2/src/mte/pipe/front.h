/*
 	Front-end of the Micro Translation Engine pipeline.
 	
	Decode human readable instructions to bit preables.

	Width is described as following:
	I8
	I16
	I32
	I64

	Currently IR compatible commands:
	ADD_[WIDTH]_IMMD 	dest, source, immd
*/

#include "mte/core.h"
#include "mte/ir.h"

axres mte_raw_to_ir(
	_in mte_raw_instr	*instr,
	_out ir_raw_instr	*buf
);

