#include "core.h"

axres ir_create(
	_in enum mte_arch 	org,
	_out ir_context 	**buf
){
	if (org <= mte_arch_min
	|| org >= mte_arch_max){
		return AX_INV_ARG;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	ir_context *ir = axmalloc(sizeof(ir_context));

	ir->call.block = ir_block;
	ir->call.unblock = ir_unblock;
	ir->call.queue = ir_queue;

	ir->org = org;
	ir->blocked = false;
	ir->queue_count = 0;
	ir->thread_count = 0;
	memcpy((void*)ir->ver, IR_VER, sizeof(IR_VER));

	*buf = ir;

	return AX_SUCC;
}

axres ir_close(
	_in ir_context 		*ir
){
	if (ir == nullptr){
		return AX_INV_ARG;
	}

	ir->call.block(ir);
	while (atomic_load_explicit(&ir->queue_count, memory_order_acquire) != 0){
		io_str(u"queue not empty");
		_sleep(20);
	}
	io_str(u"queue empty");
	axfree(ir);

	return AX_SUCC;
}

void ir_block(
	_in struct _ir_context*	ir
){
	if (ir == nullptr){
		return;
	}

	atomic_store_explicit(&ir->blocked, false, memory_order_release);
}
void ir_unblock(
	_in struct _ir_context*	ir
){
	if (ir == nullptr){
		return;
	}

	atomic_store_explicit(&ir->blocked, false, memory_order_release);
}

