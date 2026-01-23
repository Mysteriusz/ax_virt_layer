#include "vrow_thread.h"

axres vrow_thread_start(
	_in vrow_bank_thread	*thread
){
	if (thread == nullptr){
		return AX_INV_ARG;
	}

	// Set 2 indexing bits of the vrow lock to false
	atomic_fetch_and_explicit(
		&thread->vrow->lock,
		~(0b11 << (thread->bank << 1)),
		memory_order_release);

	int code = 0;

	pthread_attr_t attr;
	code = pthread_attr_init(&attr);
	if (code != 0){
		return AX_UNK_ERR;
	}

	pthread_t th = {0};
	code = pthread_create(
		&th,
		&attr,
		(void *(*)(void*))thread->func,
		&thread->func_stack);
	if (code != 0){
		return AX_UNK_ERR;
	}

	thread->pthread = th;
	
	return AX_SUCC;
}

void vrow_thread_stop(
	_in vrow_bank_thread	*thread
){
	if (thread == nullptr){
		return;
	}

	// Set 2 indexing bits of the vrow lock to true
	atomic_fetch_or_explicit(
		&thread->vrow->lock,
		0b11 << (thread->bank << 1),
		memory_order_release);
}

