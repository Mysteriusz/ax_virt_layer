#if defined(AX_WIN64)

#include <pdh.h>

#include <ax_type.h>
#include <ax_memory.h>

#include "perf.h"

double __inl_perf_tsc_freq;
double __inl_perf_cpu_freq;

#if defined(AX_UM)

void perf_measure_tsc(){
	u32 aux;
	u64 tsc_start, tsc_end;
	LARGE_INTEGER qpc_start, qpc_end, qpf;

	asrt(QueryPerformanceFrequency(&qpf));

	_mm_mfence();
	tsc_start = __rdtscp(&aux);
	asrt(QueryPerformanceCounter(&qpc_start));

	Sleep(100);

	asrt(QueryPerformanceCounter(&qpc_end));
	tsc_end = __rdtscp(&aux);

	double qpc_seconds = (double)(qpc_end.QuadPart - qpc_start.QuadPart) / (double)qpf.QuadPart;

	__inl_perf_tsc_freq = ((double)(tsc_end - tsc_start) / qpc_seconds) / 1e+6;
}

WINAPI DWORD perf_measure_cpu_stress(){
	volatile u32 i = 0;
	while(i++ < 100000);
	return 0;
};

void perf_measure_cpu(){
	DWORD id = 0;
	HANDLE thread = CreateThread(nullptr, 0, (void*)perf_measure_cpu_stress, nullptr, CREATE_SUSPENDED, &id);
	SetThreadAffinityMask(thread, 1);

	PDH_HQUERY query;
	PDH_HCOUNTER base_freq_counter;
	PDH_STATUS stat;

	PDH_FMT_COUNTERVALUE base_freq;

	stat = PdhOpenQueryA(NULL, 0, &query);
	asrt(stat == ERROR_SUCCESS);

	// Query from 0,0 since it has the highest chance of being the closest max turbo frequency
	stat = PdhAddEnglishCounterA(query, "\\Processor Information(0,0)\\Actual Frequency", 0, &base_freq_counter);
	asrt(stat == ERROR_SUCCESS);

	PdhCollectQueryData(query);
	
	ResumeThread(thread);
	WaitForSingleObject(thread, INFINITE);
	
	stat = PdhCollectQueryData(query);
	asrt(stat == ERROR_SUCCESS);

	stat = PdhGetFormattedCounterValue(base_freq_counter, PDH_FMT_DOUBLE, NULL, &base_freq);
	asrt(stat == ERROR_SUCCESS);
	
	PdhCloseQuery(query);
	CloseHandle(thread);

	__inl_perf_cpu_freq = base_freq.doubleValue;
}

#endif // defined(AX_UM)

#endif // defined(AX_WIN64)

