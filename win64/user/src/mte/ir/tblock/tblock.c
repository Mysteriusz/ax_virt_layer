#include "mte/perf.h"

#include "tblock.h"
#include "tblock_pass.h"

bool tblock_alloc(
	_in enum tblock_type 	type,
	_out tblock 		*buf
){
	if (__builtin_expect(buf == nullptr, false)){
		return nullptr;
	}

	/*
	 	TODO:
		Make some tblock heap buffer and allocate from there
	*/
	*buf = (tblock){
		.type = type,
		.ir_len = 0,
		.state = {REG_FREE},
		.liveness = {0},
		.assoc = {0},
		/*
		 	TODO:
			QUICKLY CHANGE TO STATIC BUFFER WHEN IMPLEMENTING TBLOCK TABLE
		*/
		.ir_buf = axmalloc((TBLOCK_SIZE << type) * sizeof(ir_raw_instr)),
	};
	return true;
}

bool tblock_emit(
	_in_out ir_context	*ir,
	_in_out tblock 		*block
){
	if (__builtin_expect(ir == nullptr, false)){
		return false;
	}
	if (__builtin_expect(block == nullptr, false)){
		return false;
	}

__INL_PERF_INIT

__INL_PERF_START
	struct tblock_pass_result ls = tblock_liveness_scan(ir, block);
	if (__builtin_expect(ls.res, AX_SUCC)){
		return false;
	}
__INL_PERF_END

	double r1 = (__INL_PERF_SUM / 4.2);

__INL_PERF_START
	struct tblock_pass_result rti = tblock_raw_to_ir(ir, block);
	if (__builtin_expect(rti.res, AX_SUCC)){
		return false;
	}
__INL_PERF_END

	double r2 = (__INL_PERF_SUM / 4.2);

__INL_PERF_START
	struct tblock_pass_result itr = (struct tblock_pass_result){};//tblock_ir_to_raw(ir, block);
	if (__builtin_expect(itr.res, AX_SUCC)){
		return false;
	}
__INL_PERF_END

	double r3 = (__INL_PERF_SUM / 4.2);

	printf("Average liveness pass in: %lfns\n", r1 / ls.count);
	printf("Average raw to ir pass in: %lfns\n", r2 / rti.count);
	printf("Average ir to raw pass in: %lfns\n", r3 / itr.count);
	printf("Time sum: %lfns\n", (r1 + r2 + r3));

	return true;
}

