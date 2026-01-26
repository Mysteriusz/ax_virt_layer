#if !defined(MTE_VROW_B0_INT)
#define MTE_VROW_B0_INT

#include "mte/core.h"
#include "mte/ir/ir.h"

#include "vrow.h"
#include "vrow_thread.h"
#include "scheduler.h"

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

#define init_vrow_b0_payload(ir_p, instr) ((struct vrow_b0_payload){ \
	.control.level = 0, \
	.control.context = (ir_p), \
	.payload.instr = (instr), \
	.payload.result = (ir_raw_instr){0}, \
})

void *vrow_b0_main(
	struct vrow_bank_thread_stack		*stack
);

/*
	Main execution function
*/
void vrow_b0_exec(
	vrow_desc *const			vrow,
	sched_context *const 			sched,
	volatile ir_context *const 		ir,
	volatile struct ir_context_desc *const 	ir_desc,
	volatile struct vrow_b0_payload *const 	b0
);

#endif // !defined(MTE_VROW_B0_INT)

