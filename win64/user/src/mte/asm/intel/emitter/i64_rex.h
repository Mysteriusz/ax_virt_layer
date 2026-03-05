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
	bool flip = !!(sum.operand & BIT(7));

	u8 dest = !!((sum.operand & BIT(3)) || (sum.operand & BIT(2)));
	u8 src = !!((sum.operand & BIT(1)) || (sum.operand & BIT(0)));

	u8 src_r = (flip ? src : dest) << 2;
	u8 dest_rm = (flip ? dest : src);
	u8 is_64bit = !!(sum.width & BIT(3)) << 3;
	u8 sib_ext = !!(sum.operand & BIT(5)) << 1;

	u8 rex = (I64_REX_LABEL << 4)
		| is_64bit 	// D
		| src_r 	// D | C
		| sib_ext 	// F
		| dest_rm; 	// B | A

	switch(rex){
	case 0x40: // Empty REX byte 9)
		return 0;
	default:
		return rex;
	}
}

#endif // !defined(MTE_I64_REX_INT)

