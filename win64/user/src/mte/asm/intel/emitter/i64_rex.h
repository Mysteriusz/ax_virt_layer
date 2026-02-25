#if !defined(MTE_I64_REX_INT)
#define MTE_I64_REX_INT

#include <ax_type.h>
#include <ax_io.h>

#include "i64_emit_info.h"

#define I64_REX_LABEL 0b0100

#define _rex_w(r) (r & BIT(3))
#define _rex_r(r) (r & BIT(2))
#define _rex_x(r) (r & BIT(1))
#define _rex_b(r) (r & BIT(0))

static u8 _i64_rex_resolve(
	_in struct i64_operand_sum	sum
){
	u8 rex = (I64_REX_LABEL << 4)
		| (!!(sum.width & BIT(3)) << 3)
		| (!!((sum.operand & BIT(3)) || (sum.operand & BIT(2))) << 2)
		| (!!(sum.operand & BIT(4)) << 1)
		| ((sum.operand & BIT(1)) || (sum.operand & BIT(0)));

	switch(rex){
	case 0x40: // Empty REX byte 9)
		return 0;
	default:
		return rex;
	}
}

#endif // !defined(MTE_I64_REX_INT)

