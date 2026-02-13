#include "sched.h"

_inline_avert axres sched_create(
	_in ir_context		*ir,
	_in u8			vrow_count,
	_in u16			queue_cap,
	_out sched_context	**buf
){
	if (ir == nullptr 
	|| vrow_count == 0){
		return AX_INV_ARG;
	}
	if (buf == nullptr){
		return AX_INV_BUF;
	}

	axres res = AX_SUCC;

	/*
		Create [vrow_count] amout of vrows
	*/

	// Create temporary IR context
	sched_context temp_sched = (sched_context){
		.ir = ir,
		.vrow_base = axmalloc(sizeof(vrow_desc*) * vrow_count),
		.vrow_count = vrow_count,
	};

	/*
	 	Initilize scheduler
	*/
	sched_context *sched = 
		axmalloc(sizeof(sched_context));
	memcpy(sched, &temp_sched, sizeof(sched_context));

	/*
	 	Create a sync map to fit all vrow`s
	*/
	res = sync_map_init(((vrow_count / 64) + 1) * 64, &sched->vrow_smap);
	axcheck_r(res, res, {
		ax_log(res);
		sched_delete(sched);
	});

	/*
	 	Load vrow`s with anonymous linkage to scheduler
	*/
	for (u32 i = 0; i < vrow_count; i++){
		res = vrow_create(
			ir,
			i, &sched->vrow_smap,
			&sched->vrow_base[i]);

		axcheck_r(res, res, { // TODO: Change the return code
			ax_log(res);
			sched_delete(sched);
		});
	}

	/*
	 	Create queue bitpool
	*/
	res = bitpool_create(
		64,
		BITPOOL_BUCKET_AVX512,
		BITPOOL_PERC_DEFAULT,
		&sched->queue);
	axcheck_r(res, res, { // TODO: Change the return code
		ax_log(res);
		sched_delete(sched);
	});

	pthread_attr_t attr = {0};
	pthread_attr_init(&attr);
	pthread_create(
		&sched->pthread,
		&attr,
		(void *(*)(void*))sched_main,
		sched);

	*buf = sched;

	return AX_SUCC;
}

_inline_avert void sched_delete(
	_in sched_context	*sched
){
	if(sched == nullptr){
		return;
	}

	sched_close(sched);
	while(!sched_is_terminated(sched)){
		_mm_pause();
	}

	if (sched->vrow_base != nullptr){
		// Cleanup vrow`s
		for (u8 i = 0; i < sched->vrow_count; i++){
			vrow_delete(sched->vrow_base[i]);
		}
		axfree(sched->vrow_base);
	}

	bitpool_delete(sched->queue);
	sync_map_disp(&sched->vrow_smap);
	axfree(sched);
}

bool sched_push(
	_in sched_context	*sched,
	_in vrow_payload	payload,
	_out_opt u32		*queue_index
){
	if (_sched_context_inv(sched)){
		return false;
	}

	struct bitpool_prior_load_res res =
	bitpool_prior_load( // BLOCKS IF QUEUE IS FULL!!!
		sched->queue,
		&payload,
		payload.priority);
	axcheck_r(res.code, false);

	if (queue_index != nullptr){
		*queue_index = res.index;
	}

	return true;
}

bool sched_pop(
	_in sched_context	*sched,
	_in u8			vrow_index,
	_in u32			queue_index
){
	if (_sched_context_inv(sched)){
		return AX_INV_ARG;
	}
	
	if (vrow_index >= sched->vrow_count
	|| queue_index >= sched->queue->bucket_count){
		return AX_INV_DATA;
	}

	/*
	 	Move the payload from queue to vrow
	*/

	if (!vrow_bank_load( // Blocks until vrow is free
		sched->vrow_base[vrow_index],
		0,
		*(vrow_payload*)_bitpool_index_to_bucket(sched->queue, queue_index))
	){
		return false;
	}

	/*
	 	Clear out the queue at [queue_index]
	*/
	bitpool_prior_unload(sched->queue, queue_index);

	return true;
}

void *sched_main(
	_in sched_context 	*sched
){
	bitpool_desc *queue = sched->queue;

	// Disposition array (Follows same indeces as [bitpool_desc->presence])
	u32 disp_presence[4] = {0};

	while(!sched_is_closed(sched)){
		if(!bitpool_presence_any(queue)){
			_mm_pause();
			continue;
		}

		/*
		 	Try to allocate first available vrow
		*/
		u32 vrow_i = 0;
		if (!sync_map_sig_first(
			0,
			sched->vrow_count,
			&sched->vrow_smap,
			&vrow_i)
		){
			continue;
		}

		/*
			Decide which bucket to pop from the queue
		*/
		struct bitpool_prior_unload_disp_res res = 
		bitpool_prior_unload_disp(
			queue,
			disp_presence
		);
		if (!res.succ){
			// Reverse signaling of vrow
			sync_map_sigoff(sched->vrow_smap, vrow_i);
			continue;
		}

		/*io_str(u"Presence:");
		io_i64(atomic_load(&queue->presence[3]));
		io_i64(atomic_load(&queue->presence[2]));
		io_str(u"Unload bucket:");
		io_i64(res.bucket_i);
		io_i64(res.presence_i);*/

		/*
		 	TODO: Some kind of fallback
			Pop the bucket to signaled vrow 
			(Now the vrow thread handles the rest)
		*/
		sched_pop(sched, vrow_i, res.bucket_i);
	}
	sched_terminate(sched);
	return nullptr;
}

