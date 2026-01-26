#include "vrow_thread.h"

axres vrow_thread_start(
	_in vrow_bank_thread	*thread
){
	if (thread == nullptr){
		return AX_INV_ARG;
	}

	// Unlock if thread if locked
	if (vrow_is_locked(thread->vrow, thread->bank)){
		vrow_lock_switch(thread->vrow, thread->bank);
	}

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

	// Lock if thread if unlocked
	if (!vrow_is_locked(thread->vrow, thread->bank)){
		vrow_lock_switch(thread->vrow, thread->bank);
	}
}

