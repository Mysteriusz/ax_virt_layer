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
	are recommended to be volatileto prevent compiler reordering.
*/

#define __INL_PERF_START \
	(void)__rdtsc(); \
	__asm__ __volatile__("lfence\n\t"); \
	mm_perf_l1 = __rdtsc(); \
	__asm__ __volatile__("lfence\n\t"); \

#define __INL_PERF_END \
	__asm__ __volatile__("mfence\n\t"); \
	mm_perf_l2 = __rdtscp(&mm_perf_aux); \
	__asm__ __volatile__("mfence\n\t"); \

#define __INL_PERF_INIT \
	volatile u64 mm_perf_l1 = 0, mm_perf_l2 = 0, mm_perf_empty = 0; \
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

