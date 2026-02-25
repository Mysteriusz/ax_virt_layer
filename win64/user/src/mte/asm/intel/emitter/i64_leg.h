#if !defined(MTE_I64_LEG_INT)
#define MTE_I64_LEG_INT

#include <ax_type.h>

#include <ax_type.h>
#include <ax_io.h>

#include "i64_emit_info.h"

static u8 _i64_leg_resolve(
	_in struct i64_operand_sum 	sum
){
	u8 mask = (sum.is_trunc_mem << 1) | (sum.is_16bit);
	switch(mask){
	case 0b10: // add rax, [ebx]
		return 0x67;
	case 0b01: // add ax, bx
		return 0x66;
	default:
		return 0;
	}
}

#endif // !defined(MTE_I64_LEG_INT)

