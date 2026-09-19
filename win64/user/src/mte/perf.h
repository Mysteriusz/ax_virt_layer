/*
 	Architecture performance counter interface.
*/

#if !defined(MTE_PERF_INT)
#define MTE_PERF_INT

#include <ax_type.h>

#define __INL_PERF_SUM ((mm_perf_l2 - mm_perf_l1) - mm_perf_empty)

/*
 	---------------------------------- NOTICE ----------------------------------

	__INL_PERF_*_ASM is deprecated for now
*/

/*#define __INL_PERF_INIT_ASM \
	volatile i64 mm_perf_l1 = 0, mm_perf_l2 = 0, mm_perf_empty = 0; \
	__asm__ __volatile__( \
		"cpuid\n\t" \
		"cpuid\n\t" \
		"cpuid\n\t" \
		"rdtsc\n\t" \
		"shl 		$32, %%rdx\n\t" \
		"orq 		%%rdx, %%rax\n\t" \
		"movq 		%%rax, %%r8\n\t" \
		"cpuid\n\t" \
		"rdtsc\n\t" \
		"shl 		$32, %%rdx\n\t" \
		"orq 		%%rdx, %%rax\n\t" \
		"subq 		%%r8, %%rax\n\t" \
		: "=a"(mm_perf_empty) \
		: \
		: "rbx", "rcx", "rdx", "r8", "memory" \
	);

#define __INL_PERF_START_ASM \
	__asm__ __volatile__( \
		"cpuid\n\t" \
		"cpuid\n\t" \
		"cpuid\n\t" \
		"rdtsc\n\t" \
		"shl 		$32, %%rdx\n\t" \
		"orq 		%%rdx, %%rax\n\t" \
		: "=a"(mm_perf_l1) \
		: \
		: "rbx", "rcx", "rdx", "r8", "memory" \
	);

#define __INL_PERF_END_ASM \
	__asm__ __volatile__( \
		"cpuid\n\t" \
		"rdtsc\n\t" \
		"shl 		$32, %%rdx\n\t" \
		"orq 		%%rdx, %%rax\n\t" \
		: "=a"(mm_perf_l2) \
		: \
		: "rbx", "rcx", "rdx", "memory" \
	);*/

#define __INL_PERF_INIT \
	volatile i64 mm_perf_l1 = 0, mm_perf_l2 = 0, mm_perf_empty = 0; \
	u32 mm_perf_aux = 0;

#define __INL_PERF_START \
	_mm_mfence(); \
	mm_perf_l1 = __rdtscp(&mm_perf_aux);

#define __INL_PERF_END \
	_mm_lfence(); \
	mm_perf_l2 = __rdtscp(&mm_perf_aux);

void perf_measure_cpu();
void perf_measure_tsc();

/*
 	TODO:
	Make both frequencies atomic
*/
extern double __inl_perf_cpu_freq; // In MHz
extern double __inl_perf_tsc_freq; // In MHz

static double _inl_perf_cpu_freq(){
	if (__inl_perf_cpu_freq == 0){
		perf_measure_cpu();
	}
	return __inl_perf_cpu_freq;
}
static double _inl_perf_tsc_freq(){
	if (__inl_perf_tsc_freq == 0){
		perf_measure_tsc();
	}
	return __inl_perf_tsc_freq;
}
static double _inl_perf_cpu_tsc_ratio(){
	return _inl_perf_cpu_freq() / _inl_perf_tsc_freq();
}

#define __INL_PERF_LOG \
	printf("TSC frequency in MHz: %lf\n", _inl_perf_tsc_freq() / 1000); \
	printf("CPU frequency in MHz: %lf\n", _inl_perf_cpu_freq() / 1000); \
	printf("Empty in ns: %lf\n", (mm_perf_empty * _inl_perf_cpu_tsc_ratio()) / (_inl_perf_cpu_freq() / 1000)); \
	printf("Time in cycles: %lf\n", __INL_PERF_SUM * _inl_perf_cpu_tsc_ratio()); \
	printf("Time in ns: %lf\n", (__INL_PERF_SUM * _inl_perf_cpu_tsc_ratio()) / (_inl_perf_cpu_freq() / 1000));

/*#define __INL_PERF_LOG_MS \
	printf("Time in ms: %lf\n", (__INL_PERF_SUM * 1e-6) / 4.2);*/

#endif // !defined(MTE_PERF_INT)

