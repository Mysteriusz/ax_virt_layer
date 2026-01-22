#include "intel64_ir.h"

axres intel_raw_to_ir(
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

