#if !defined(MTE_SCHEDULER_INT)
#define MTE_SCHEDULER_INT

#include <pthread.h>

#include "mte/ir/ir.h"
#include "mte/core.h"

#include "vrow.h"
#include "sync_map.h"

typedef struct _sched_context{
	const ir_context *const	ir;
	sync_map_desc		vrow_smap;
	vrow_desc **const 	vrow_base;
	u8 			vrow_count;
	void 			*queue_base;
} sched_context;

_inline_avert axres sched_create(
	_in ir_context		*ir,
	_in u8			vrow_count,
	_in u16			queue_cap,
	_out sched_context	**buf
);

_inline_avert void sched_delete(
	_in sched_context	*sched
);

void *sched_main(
	_in sched_context 	*sched
);

/*
 	Blocking schedule of a payload to queue
*/
void sched_push(
	_in sched_context	*sched,
	_in vrow_payload	payload
);

#endif // !defined(MTE_SCHEDULER_INT)

