#include "mte/perf.h"

#include "tblock.h"
#include "tblock_pass.h"

_free tblock* tblock_alloc(
	_in const enum tblock_type 	type
){
	/*
		TODO:
		CHANGE TO STATIC BUFFER WHEN IMPLEMENTING TBLOCK TABLE
	*/
	tblock *out = axmalloc(sizeof(tblock));

	_fill_array(out->state, sizeof(out->state) / sizeof(asm_reg_state),
		&(asm_reg_state){REG_FREE}, sizeof(asm_reg_state));

	_fill_array((u8*)out->liveness, sizeof(out->liveness) / sizeof(asm_reg_liveness),
		&(asm_reg_liveness){0}, sizeof(asm_reg_liveness));

	_fill_array((u8*)out->assoc, sizeof(out->assoc) / sizeof(asm_reg_assoc),
		&(asm_reg_assoc){0}, sizeof(asm_reg_assoc));

	out->ir_cnt = 0;

	*(u32*)&out->ir_buf_len = tblock_frag_calc(type);
	*(ir_raw_instr**)&out->ir_buf = axmalloc(tblock_frag_calc(type));
	*(enum tblock_type*)&out->type = type;

	return out;
}

bool tblock_emit(
	_in_out ir_context	*ir,
	_in_out tblock 		*const block
){
	if (__builtin_expect(ir == nullptr, false)){
		return false;
	}
	if (__builtin_expect(block == nullptr, false)){
		return false;
	}

__INL_PERF_INIT;

__INL_PERF_START;

	auto ls = tblock_liveness_scan(ir, block);
	if (__builtin_expect(ls.res, AX_SUCC)){
		return false;
	}

__INL_PERF_END;

	double r1 = __INL_PERF_SUM * _inl_perf_cpu_tsc_ratio();

__INL_PERF_START;

	auto rti = tblock_raw_to_ir(ir, block);
	if (__builtin_expect(rti.res, AX_SUCC)){
		return false;
	}
__INL_PERF_END;

	double r2 = __INL_PERF_SUM * _inl_perf_cpu_tsc_ratio();

__INL_PERF_START;

	auto itr = tblock_ir_to_raw(ir, block);
	if (__builtin_expect(itr.res, AX_SUCC)){
		return false;
	}
__INL_PERF_END;

	double r3 = __INL_PERF_SUM * _inl_perf_cpu_tsc_ratio();

	printf("IR instructions generated: %u\n", block->ir_cnt);
	printf("Liveness pass in: %lf ns\n", r1 / (_inl_perf_cpu_freq() / 1000));
	printf("Raw to ir pass in: %lf ns\n", r2 / (_inl_perf_cpu_freq() / 1000));
	printf("IR to raw pass in: %lf ns\n", r3 / (_inl_perf_cpu_freq() / 1000));
	printf("Time sum: %lf ns\n", (r1 + r2 + r3) / (_inl_perf_cpu_freq() / 1000));

	return true;
}

