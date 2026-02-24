#if !defined(MTE_I64_REX_INT)
#define MTE_I64_REX_INT

#include <ax_type.h>
#include <ax_io.h>

#include "i64_emit_info.h"

#define I64_REX_LABEL 0b0100

static u8 _i64_rex_resolve(
	_in struct i64_operand_sum	sum
){
	return (I64_REX_LABEL << 4)
		| (sum.is_64bit << 3) 
		| ((sum.is_r1_ext || sum.is_r0_ext_mem) << 2)
		| (sum.is_sib_ext << 1)
		| (sum.is_r0_ext || sum.is_r1_ext_mem);
}

#endif // !defined(MTE_I64_REX_INT)

