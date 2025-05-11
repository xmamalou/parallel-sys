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

#define __USE_XOPEN2K

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#define itimerspec linux_itimerspec
#include <linux/time.h>
#undef itimerspec

// --- TYPES --- //

typedef struct Benchmark {
    struct timespec start; 
    struct timespec end;
} Benchmark;

typedef struct Log {
    FILE* file_p;
} Log;

typedef struct Barrier {
    pthread_mutex_t mutex;
    pthread_cond_t  condition;
    uint32_t        claimers;
    uint32_t        curr_claimers;
} Barrier;

typedef struct Matrix {
    uint32_t dims[2];
    double*  matrix;
} Matrix;

// --- CONSTANTS --- //

const uint64_t nsecs_in_sec = 1000000000;

// --- FUNCTION DEFINITIONS --- //

BENCHMARK_T start_benchmark() 
{
    const Benchmark* benchmark_p = (Benchmark*)calloc(1, sizeof(Benchmark));
    clock_gettime(CLOCK_REALTIME, &benchmark_p->start);

    return (BENCHMARK_T)benchmark_p;
}

uint64_t stop_benchmark(BENCHMARK_T handle)
{
    const Benchmark* benchmark_p = (Benchmark*)handle;
    clock_gettime(CLOCK_REALTIME, &benchmark_p->end);

    const uint64_t time =
            benchmark_p->end.tv_sec*nsecs_in_sec + benchmark_p->end.tv_nsec
            - benchmark_p->start.tv_sec*nsecs_in_sec - benchmark_p->start.tv_nsec;
    free((void*)benchmark_p);

    return time;
}

LOG_T open_log(
        const char* const path,
        const bool        do_append)
{
    const FILE* file_p = fopen(
            path, 
            do_append ? "a" : "w");
    Log* log_p = (Log*)calloc(1, sizeof(Log));
    log_p->file_p = file_p;

    return (LOG_T)log_p;
}

void write_log(
        LOG_T handle,
        const char* const text)
{
    const Log* log_p = (Log*)handle;
    fprintf(
            log_p->file_p,
            text);
}

void close_log(LOG_T handle)
{
    const Log* log_p = (Log*)handle;
    fclose(log_p->file_p);
    free((void*)log_p);

    return;
}

BARRIER_T create_barrier(const uint32_t holder_count)
{
    Barrier* barrier_p = (Barrier*)calloc(1, sizeof(Barrier));
    pthread_mutex_init(
            &barrier_p->mutex,
            NULL);
    pthread_cond_init(
            &barrier_p->condition,
            NULL);

    barrier_p->claimers = holder_count;
    barrier_p->curr_claimers = 0;

    return (BARRIER_T)barrier_p;
}

void waitfor_barrier(const BARRIER_T barrier_h)
{
    Barrier* barrier_p = (Barrier*)barrier_h;

    pthread_mutex_lock(&barrier_p->mutex);
    barrier_p->curr_claimers++;

    if (barrier_p->curr_claimers == barrier_p->claimers)
    {
        barrier_p->curr_claimers = 0;
        pthread_cond_broadcast(&barrier_p->condition);
    } else {
        pthread_cond_wait(
                &barrier_p->condition,
                &barrier_p->mutex);
    }
    pthread_mutex_unlock(&barrier_p->mutex);
}

void destroy_barrier(BARRIER_T barrier_h)
{
    Barrier* barrier_p = (Barrier*)barrier_h;
    pthread_mutex_destroy(&barrier_p->mutex);
    pthread_cond_destroy(&barrier_p->condition);

    free((void*)barrier_h);
}