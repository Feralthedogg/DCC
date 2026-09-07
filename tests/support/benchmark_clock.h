#ifndef DCC_TEST_BENCHMARK_CLOCK_H
#define DCC_TEST_BENCHMARK_CLOCK_H

#include <time.h>
#if defined(_WIN32)
#include <windows.h>
#endif

static int dcc_benchmark_now(struct timespec *value) {
#if defined(_WIN32)
    LARGE_INTEGER frequency, counter;
    if (!QueryPerformanceFrequency(&frequency) || frequency.QuadPart <= 0 ||
        !QueryPerformanceCounter(&counter) || counter.QuadPart < 0) return 1;
    value->tv_sec = (time_t)(counter.QuadPart / frequency.QuadPart);
    value->tv_nsec = (long)((double)(counter.QuadPart % frequency.QuadPart) *
                            (1000000000.0 / (double)frequency.QuadPart));
    return 0;
#else
    return clock_gettime(CLOCK_MONOTONIC, value);
#endif
}

#endif
