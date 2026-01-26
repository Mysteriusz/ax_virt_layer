#if !defined(MTE_SCHEDULER_INT)
#define MTE_SCHEDULER_INT

#include "mte/core.h"

#include "vrow.h"
#include "vrow_thread.h"

typedef struct _sched_context{
	vrow_desc *const 	vrow;
} sched_context;

bool sched_instr(
	_in mte_raw_instr 	instr,
	_in vrow_desc		*vrow
);

#endif // !defined(MTE_SCHEDULER_INT)

