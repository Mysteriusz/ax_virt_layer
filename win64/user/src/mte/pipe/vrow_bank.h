#if !defined(MTE_VROW_BANK_INT)
#define MTE_VROW_BANK_INT

#include "mte/core.h"
#include "mte/ir/ir.h"

#include "vrow.h"

/*
 	Bank 0 processing
*/
#define VROW_B0_OFFSET 0

struct vrow_b0_payload{ _align(16)
	struct{
		u8 		level; // Always 0 for b0
		u8 		reserved[7];
		ir_context 	*context; // 8 bytes
	} control;
	struct{
		mte_raw_instr 	instr; // 16 bytes
		ir_raw_instr 	result; // 16 bytes
		u8 		reserved[16];
	} payload;
};

#define init_vrow_b0_payload(ir_p, i_p) ((struct vrow_b0_payload){ \
	.control.level = 0, \
	.control.context = (ir_p), \
	.payload.instr = (i_p), \
	.payload.result = (ir_raw_instr){0}, \
})

bool vrow_bank_0_proc(
	_in vrow_desc		*vrow,
	_in org_to_ir_call	func
);

#endif // !defined(MTE_VROW_BANK_INT)

