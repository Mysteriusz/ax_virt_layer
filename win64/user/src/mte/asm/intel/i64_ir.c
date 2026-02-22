#include "i64_ir.h"

axres i64_raw_to_ir(
	_in i64_mte_raw_instr	instr,
	_in_out ir_raw_instr		*buf // Initialize before use by init_ir_raw_instr
){
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	/*switch(_i64_check_type(instr)){
	default:
		return 0;
	}*/
	return 0;
}

axres i64_ir_to_raw(
	_in ir_raw_instr		instr,
	_in_out i64_mte_raw_instr	*buf // Initialize before use by init_ir_raw_instr
){
	if (buf == nullptr){
		return AX_INV_BUF;
	}
	return AX_SUCC;
}

