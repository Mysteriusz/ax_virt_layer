#if !defined(MTE_SCHEDULER_INT)
#define MTE_SCHEDULER_INT

#include <pthread.h>

#include "mte/ir/ir.h"
#include "mte/core.h"

#include "vrow.h"
#include "sync_map.h"
#include "bitpool.h"

typedef struct _sched_context{
	const ir_context *const	ir;
	bitpool_desc 		*queue;
	sync_map_desc		vrow_smap;
	vrow_desc **const 	vrow_base; // Array of vrow pointer`s
	const u8 		vrow_count; // Count of processing vrow`s
	/*
	 	Describes state of the scheduler.

		Bit format follows:
			- 000000TC
			(T) -> Thread terminated;
			(C) -> Closed;
	*/
	_Atomic(u8)		state;
	pthread_t		pthread;
} sched_context;

#define sched_is_terminated(sched_p) \
	(atomic_load_explicit(&(sched_p)->state, memory_order_acquire) & 0x02)

#define sched_terminate(sched_p) \
	(atomic_fetch_or_explicit(&(sched_p)->state, 0x02, memory_order_release))

#define sched_close(sched_p) \
	(atomic_fetch_or_explicit(&(sched_p)->state, 0x01, memory_order_release))

#define sched_is_closed(sched_p) \
	(atomic_load_explicit(&(sched_p)->state, memory_order_acquire) & 0x01)


_inline_force bool _sched_context_inv(
	_in sched_context	*sched
){
	if (sched == nullptr){
		return true;
	}

	// Check if queue is not corrupted
	asrt(sched->ir != nullptr);
	asrt(sched->queue != nullptr);
	asrt(sched->queue->bucket_size == sizeof(vrow_payload));

	return false;
}

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
 	Blocking schedule of a payload to queue and processing

	TODO:

	Scheduler currently uses bitpool with thread safe sync_map 
	which may not be ideal for performance.
	
	In the future this should be changed.
*/
bool sched_push(
	_in sched_context	*sched,
	_in vrow_payload	payload,
	_out_opt u32		*queue_index // Index in the [sched->queue] which took the payload
);

bool sched_pop(
	_in sched_context	*sched,
	_in u8			vrow_index,  // Index in the [sched->vrow_base] to load the payload
	_in u32			queue_index // Index in the [sched->queue] to pop
);

#endif // !defined(MTE_SCHEDULER_INT)

