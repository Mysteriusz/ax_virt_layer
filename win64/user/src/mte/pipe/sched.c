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
		.queue_base = nullptr,
	};

	/*
	 	Initilize scheduler
	*/
	sched_context *sched = 
		axmalloc(sizeof(sched_context));
	memcpy(sched, &temp_sched, sizeof(sched_context));

	/*
	 	Load vrow`s with linkage to scheduler
	*/
	sync_map_init(sizeof(u64) * (vrow_count / 8), &sched->vrow_smap);
	for (u8 i = 0; i < vrow_count; i++){
		sched->vrow_smap.index = i;
		res = vrow_create(
			ir,
			&sched->vrow_smap,
			&sched->vrow_base[i]);

		axcheck_r(res, res, { // TODO: Change the return code
			sched->vrow_count = i;
			sched_delete(sched);
		});
	}
	sched->vrow_smap.index = 0;

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

	// TODO: When queue/pool structure ready change this
	if (sched->queue_base != nullptr){
		axfree(sched->queue_base);
	}

	//sync_map_disp(&sched->vrow_smap);
	axfree(sched);
}

void *sched_main(
	_in sched_context 	*sched
){
	while(0){
		_mm_pause();
	}
	sched_delete(sched);
	return nullptr;
}

void sched_push(
	_in sched_context	*sched,
	_in vrow_payload	payload
){
	return;
}

