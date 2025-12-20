#include <pthread.h>

#include "ir/core.h"
#include "ir/disp.h"

struct ir_queue_async_stack{
	_in struct _ir_context	*ir;
	_in mte_raw_instr 	*instr;
	_out ir_raw_instr 	**buf;
};
void *ir_queue_async(
	_free void 		*arg
){
	struct ir_queue_async_stack *stack = arg;

	// Frontend pipeline
	/*ir_disp_conv(
		stack->ir->org,
		stack->instr,
		true);*/

	// Free stack (marked by _free)
	axfree(stack);

	// Decrease queue count (it was processed)
	atomic_fetch_sub_explicit(
		&stack->ir->queue_count,
		1, memory_order_release);

	return nullptr;
}

axres ir_queue(
	_in struct _ir_context	*ir,
	_in mte_raw_instr 	*instr,
	_out ir_raw_instr 	**buf
){
	if (ir == nullptr
	|| instr == nullptr){
		return AX_INV_ARG;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}
	if (atomic_load(&ir->blocked) == true){
		return AX_IR_BLOCKED;
	}

	struct ir_queue_async_stack* q_stack = axmalloc(sizeof(struct ir_queue_async_stack));
	q_stack->ir = ir;
	q_stack->instr = instr;
	q_stack->buf = buf;

	pthread_t q_thread = {0};
	pthread_create(
		&q_thread,
		0, 
		ir_queue_async,
		q_stack);

	// Increase queue count for all threads
	atomic_fetch_add_explicit(
		&ir->queue_count,
		1, memory_order_release);

	return AX_SUCC;
}

