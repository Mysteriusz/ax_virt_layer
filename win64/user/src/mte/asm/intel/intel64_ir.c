#include "intel64_ir.h"

axres intel64_raw_to_ir(
	_in intel64_mte_raw_instr	instr,
	_in_out ir_raw_instr		*buf // Initialize before use by init_ir_raw_instr
){
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	/*switch(_intel64_check_type(instr)){
	default:
		return 0;
	}*/
	return 0;
}

axres intel64_ir_to_raw(
	_in ir_raw_instr		instr,
	_in_out intel64_mte_raw_instr	*buf // Initialize before use by init_ir_raw_instr
){
	if (buf == nullptr){
		return AX_INV_BUF;
	}
	return AX_SUCC;
}

