/*
 	Architecture performance counter interface.
*/

#if !defined(MTE_PERF_INT)
#define MTE_PERF_INT

#include <ax_type.h>

#define __INL_PERF_SUM ((mm_perf_l2 - mm_perf_l1) - mm_perf_empty)

/*
 	---------------------------------- NOTICE ----------------------------------

 	All elements between __INL_PERF_START and __INL_PERF_END
	are recommended to be volatile to prevent compiler reordering.
*/

/*
 	Currently there is a chance (approximately 1/10)
	for measurement to return negative number! (NO IDEA HOW TO MEASURE CPUID MORE ACCURATELY)
*/
#define __INL_PERF_INIT_ASM \
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
	);

#define __INL_PERF_START \
	__asm__ __volatile__("mfence\n\t" ::: "memory"); \
	mm_perf_l1 = __rdtsc(); \
	__asm__ __volatile__("mfence\n\t" ::: "memory");

#define __INL_PERF_END \
	__asm__ __volatile__("mfence\n\t" ::: "memory"); \
	mm_perf_l2 = __rdtscp(&mm_perf_aux); \
	__asm__ __volatile__("mfence\n\t" ::: "memory");

#define __INL_PERF_INIT \
	volatile i64 mm_perf_l1 = 0, mm_perf_l2 = 0, mm_perf_empty = 0; \
	u32 mm_perf_aux = 0; \
	__INL_PERF_START \
	__INL_PERF_END \
	mm_perf_empty = __INL_PERF_SUM;

#define __INL_PERF_LOG \
	printf("Empty in ns: %lf\n", (mm_perf_empty / 4.2)); \
	printf("Time in ns: %lf\n", (__INL_PERF_SUM / 4.2));

#define __INL_PERF_LOG_MS \
	printf("Time in ms: %lf\n", ((__INL_PERF_SUM * 1e-6) / 4.2));

#endif // !defined(MTE_PERF_INT)

