/*
    Parallel Systems Exercise Batch 1 -- Solutions to Batch 1 of Exercises for the Parallel
    Systems Course of the "Computer Engineering" Masters Programme of NKUA
    Copyright (C) 2025 Christoforos-Marios Mamaloukas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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