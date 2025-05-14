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

#include "exercises.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <linux/limits.h>
#include <omp.h>

#include "utility.h"
#include "macros.h"

// -- TYPES --- //

typedef enum Which {
    SERIAL   = 0,
    PTHREADS = 1,
    OPENMP   = 2,
} Which;

typedef struct Options {
    uint64_t throw_count;
    uint32_t job_count;
    Which    which_method;
    char     data_path[PATH_MAX];
    uint32_t tries;
} Options;

typedef struct ThreadParams {
    uint64_t throw_count;
} ThreadParams;

// --- CONSTANTS --- //

static const char* exercise_type[] = {
    "serial",
    "parallel with pthreads",
    "parallel with OpenMP",
};

static const uint64_t nsec_to_sec_factor  = 1000000000;
static const uint64_t nsec_to_msec_factor = 1000000;

// --- GLOBALS --- //

/// @brief total amount of successful throws (i.e. inside the circle)
static uint64_t        succ_throws_g = 0;
static pthread_mutex_t throws_mtx;

// --- FUNCTION DECLARATIONS --- //

FLAG_READER_DECL();

EXERCISE_IMPLM_DECL(pi_calc_serial);
EXERCISE_IMPLM_DECL(pi_calc_parallel);
EXERCISE_IMPLM_DECL(pi_calc_openmp);

/// @brief Function that calculates amount of successful throws
/// @param args Pointer to thread parametrization structure
/// @return NULL
CALLBACK_DECL(succ_throws);

// --- FUNCTION DEFINITIONS --- //

void pi_calc(
        char** flags, uint32_t flag_count) 
{
    Options options = {
        .job_count    = 1,
        .throw_count  = 10, 
        .which_method = PTHREADS,
        .data_path    = "",
        .tries        = 1,
    };
    read_flags(
            flags, flag_count,
            &options);

    // If the user wants to use stdout to output data, we suppress any messages to
    // allow only the output that would normally be logged to show up
    if ( strcmp(options.data_path, "") != 0 ) 
    {
        printf("\x1b[33mParallel Systems Postgrad Course -- Project 1 -- Christoforos-Marios Mamaloukas\n\n"
                "--- EXERCISE 1 (Calculation of π) ---\n"
                "* You selected %s execution\n"
                "* You want %u jobs\n"
                "* You want %lu throws\n"
                "* Data will be saved to %s\n"
                "* The experiment will be run %d tries\n"
                "-------------------------------------\n\n\x1b[0m", 
                exercise_type[options.which_method],
                options.job_count, 
                options.throw_count,
                options.data_path,
                options.tries);
    }

    typedef void (*Implementations)(const Options*);
    Implementations functions[] = {
        &pi_calc_serial,
        &pi_calc_parallel,
        &pi_calc_openmp,
    };
    functions[options.which_method](&options);

    return;
}

FLAG_READER(options_p)
{
    // Which implementation to use
    SET_FLAG("-fserial", options_p->which_method, SERIAL);
    SET_FLAG("-fs", options_p->which_method, SERIAL);

    SET_FLAG("-fpthreads", options_p->which_method, PTHREADS);
    SET_FLAG("-fp", options_p->which_method, PTHREADS);

    SET_FLAG("-fomp", options_p->which_method, OPENMP);

    SET_FLAG_WITH_NUM("-fthrows=", options_p->throw_count, ll);
    SET_FLAG_WITH_NUM("-fn=", options_p->throw_count, ll);

    END_FLAG_READER();
}

inline static void pi_calc_serial(const Options* options_p) 
{
    uint64_t avg_succ_throws = 0; // throws inside the circle
    uint64_t avg_time        = 0;
    uint32_t seed        = 10000;

    // we run the experiment as many times as it is requested; 
    // to gain a bit of speed, we do the experiments in parallel, since they
    // are independent from each other (and because we don't care to measure
    // multithreaded performance)
    #pragma omp parallel for num_threads(options_p->tries) \
            reduction(+: avg_succ_throws, avg_time)
    for (uint32_t j = 0; j < options_p->tries; j++) 
    {
        BENCHMARK_T bench_h = start_benchmark();
        uint64_t succ_throws = 0;
        for (uint64_t i = 0; i < options_p->throw_count; i++)
        {
            double x = (double)rand_r(&seed)/(double)RAND_MAX, 
                    y = (double)rand_r(&seed)/(double)RAND_MAX;

            if (x*x + y*y <= 1.0) 
            {
                succ_throws++; // we will average this later
            }
        }

        avg_succ_throws += succ_throws;
        avg_time        += stop_benchmark(bench_h);
    }

    avg_succ_throws /= options_p->tries;
    avg_time        /= options_p->tries;

    float pi_val = 4*avg_succ_throws/(double)options_p->throw_count;

    LOG(
            "[EXERCISE 1]\ntype = serial\njobs = %d\nthrows = %d\nπ = %f\ntime = %f\n",
            options_p->job_count,
            options_p->throw_count,
            pi_val, 
            (double)avg_time/(double)nsec_to_msec_factor);
}

inline static void pi_calc_parallel(const Options* options_p) 
{
    uint64_t throws_per_job = options_p->throw_count/options_p->job_count;
    ThreadParams parameters = {
        .throw_count = throws_per_job
    };
    pthread_t* threads = calloc(
            options_p->job_count,
            sizeof(pthread_t));
    pthread_mutex_init(
            &throws_mtx,
            NULL);
    
    uint64_t avg_time = 0;
    // we avoid multithreading the loop in this scenario
    for (uint32_t j = 0; j < options_p->tries; j++) 
    {
        for (uint32_t i = 0; i < options_p->job_count; i++)
        {
            uint32_t err = pthread_create(
                    &threads[i],
                    NULL,
                    &succ_throws_callback,
                    &parameters);
        }
        // now we wait for all the threads to finish
        BENCHMARK_T bench_h = start_benchmark();
        for (uint32_t i = 0; i < options_p->job_count; i++)
        {
            pthread_join(
                    threads[i],
                    NULL);
            threads[i] == NULL;
        }
        avg_time += stop_benchmark(bench_h);
    }

    free(threads);
    
    avg_time      /= options_p->tries;
    succ_throws_g /= options_p->tries;
    float pi_val   = 4*succ_throws_g/(double)options_p->throw_count;

    LOG(
        "[EXERCISE 1]\ntype = serial\njobs = %d\nthrows = %d\nπ = %f\ntime = %f\n",
        options_p->job_count,
        options_p->throw_count,
        pi_val, 
        (double)avg_time/(double)nsec_to_msec_factor);

}

inline static void pi_calc_openmp(const Options* options_p)
{
    uint64_t throws_per_job  = options_p->throw_count/options_p->job_count;
    uint64_t avg_succ_throws = 0;
    uint64_t avg_time        = 0;
    
    for (uint32_t j = 0; j < options_p->tries; j++)
    {
        BENCHMARK_T bench_h = start_benchmark();
        #pragma omp parallel for num_threads(options_p->job_count) \
                reduction(+: avg_succ_throws)
        for (uint32_t k = 0; k < options_p->job_count; k++)
        {
            uint64_t succ_throws = 0;
            uint32_t seed        = 10000;
            for (uint64_t i = 0; i < throws_per_job; i++)
            {
                double x = (double)rand_r(&seed)/(double)RAND_MAX, 
                        y = (double)rand_r(&seed)/(double)RAND_MAX;

                if (x*x + y*y <= 1.0) 
                {
                    succ_throws++;
                }
            }

            avg_succ_throws += succ_throws;
        }
        avg_time += stop_benchmark(bench_h);
    }

    avg_succ_throws /= options_p->tries;
    float pi_val     = 4*avg_succ_throws/(double)options_p->throw_count;
    avg_time        /= options_p->tries;
    LOG(
        "[EXERCISE 1]\ntype = serial\njobs = %d\nthrows = %d\nπ = %f\ntime = %f\n",
        options_p->job_count,
        options_p->throw_count,
        pi_val, 
        (double)avg_time/(double)nsec_to_msec_factor);
}

void* succ_throws_callback(void* args)
{
    ThreadParams* params = (ThreadParams*)args;

    uint64_t succ_throws = 0;
    uint32_t seed        = 10000;
    for (uint64_t i = 0; i < params->throw_count; i++)
    {
        double x = (double)rand_r(&seed)/(double)RAND_MAX, 
                y = (double)rand_r(&seed)/(double)RAND_MAX;

        if (x*x + y*y <= 1.0) 
        {
            succ_throws++;
        }
    }

    pthread_mutex_lock(&throws_mtx);
    succ_throws_g += succ_throws;
    pthread_mutex_unlock(&throws_mtx);

    return NULL;
}