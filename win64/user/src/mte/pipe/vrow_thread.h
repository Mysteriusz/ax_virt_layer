#if !defined(MTE_VROW_THREAD_INT)
#define MTE_VROW_THREAD_INT

#include <pthread.h>
#include <stdatomic.h>

#include "vrow.h"
#include "scheduler.h"

struct vrow_bank_thread_stack{
	sched_context 	*sched;
	u8		bank;
};
typedef struct _vrow_bank_thread{
	void 				*(*func)(struct vrow_bank_thread_stack*);
	struct vrow_bank_thread_stack 	func_stack;
	pthread_t 			pthread;
} vrow_bank_thread;
#define init_vrow_bank_thread(s_p, b, f_p) \
	((vrow_bank_thread){ \
		.func = (f_p), \
		.func_stack = {.bank = (b), .sched = (s_p)}, \
	})

/*
 	Create and run vrow_bank_thread object.
	Unlocks thread in vrow if locked.
*/
axres vrow_thread_start(
	_in vrow_bank_thread	*thread
);

/*
	Lock vrow_bank_thread`s vrow bank to locked.
*/
void vrow_thread_stop(
	_in vrow_bank_thread	*thread
);

#endif // !defined(MTE_VROW_THREAD_INT)

