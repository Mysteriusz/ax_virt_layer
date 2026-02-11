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

	*buf = sched;

	return AX_SUCC;
}

_inline_avert void sched_delete(
	_in sched_context	*sched
){
	if(sched == nullptr){
		return;
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

void *sched_main(
	_in sched_context 	*sched
){
	while(1){
		_mm_pause();
	}
	sched_delete(sched);
	return nullptr;
}

bool sched_push(
	_in sched_context	*sched,
	_in vrow_payload	payload,
	_out_opt u32		*index
){
	if (sched == nullptr){
		return false;
	}

	// Check if queue is not corrupted
	asrt(sched->queue != nullptr);
	asrt(sched->queue->bucket_size == sizeof(vrow_payload));

	struct bitpool_prior_load_res res = bitpool_prior_load( // BLOCKS IF QUEUE IS FULL!!!
		sched->queue,
		&payload,
		payload.priority);
	axcheck_r(res.code, false);

	if (index != nullptr){
		*index = res.index;
	}

	return true;
}

