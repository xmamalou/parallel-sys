#include "utility.h"

#include <stdlib.h>
#include <time.h>
#define itimerspec linux_itimerspec
#include <linux/time.h>
#undef itimerspec

// --- TYPES --- //

typedef struct Benchmark {
    struct timespec start; 
    struct timespec end;
} Benchmark;

// --- CONSTANTS --- //

const uint64_t nsecs_in_sec = 1000000000;

// --- FUNCTION DEFINITIONS --- //

BenchmarkHandle start_benchmark() 
{
    Benchmark* benchmark_p = (Benchmark*)calloc(1, sizeof(Benchmark));
    clock_gettime(CLOCK_REALTIME, &benchmark_p->start);

    return (BenchmarkHandle)benchmark_p;
}

uint64_t stop_benchmark(BenchmarkHandle handle)
{
    Benchmark* benchmark_p = (Benchmark*)handle;
    clock_gettime(CLOCK_REALTIME, &benchmark_p->end);

    uint64_t time =
            benchmark_p->end.tv_sec*nsecs_in_sec + benchmark_p->end.tv_nsec
            - benchmark_p->start.tv_sec*nsecs_in_sec - benchmark_p->start.tv_nsec;
    free((void*)benchmark_p);

    return time;
}