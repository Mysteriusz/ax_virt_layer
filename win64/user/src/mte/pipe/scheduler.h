#if !defined(MTE_SCHEDULER_INT)
#define MTE_SCHEDULER_INT

#include <pthread.h>

#include "mte/ir/ir.h"
#include "mte/core.h"

#include "vrow.h"

typedef struct _sched_context{
	ir_context *const	ir;
	vrow_desc *const 	vrow;
	const u8 		vrow_act;
	pthread_t 		thread;
} sched_context;

_inline_avert axres sched_create(
	_in ir_context		*ir,
	_in vrow_desc		*vrow,
	_out sched_context	**buf
);

_inline_avert void sched_delete(
	_in sched_context	*sched
);

void *sched_main(
	sched_context 		*sched
);

void sched_to_next(
	_in sched_context 	*sched,
	_in u8 			bank_i
);
void sched_to_queue(
	void
);

#endif // !defined(MTE_SCHEDULER_INT)

