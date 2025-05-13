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
#include <stdlib.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <linux/limits.h>

#include "utility.h"
#include "macros.h"

// --- TYPES --- //

typedef enum Which {
    NO_SYNC = 0,
    MUTEX = 1,
    ATOMIC = 2,
} Which;

typedef struct Options {
    Which    which_method;
    uint32_t tries;
    uint32_t job_count;
    uint32_t incr_times;
    char     data_path[PATH_MAX];
} Options;

// --- CONSTANTS --- //

static const char* exercise_type[] = {
    "no synchronization",
    "locks",
    "atomics",
};

static const uint64_t nsec_to_sec_factor  = 1000000000;
static const uint64_t nsec_to_msec_factor = 1000000;

// --- GLOBALS --- //

static uint64_t*         incremented_gs = NULL;
static _Atomic uint64_t* incremented_atom_gs = NULL;
static pthread_mutex_t   shared_var_mtx;

// --- FUNCTION DECLARATIONS --- //

FLAG_READER_DECL();

EXERCISE_IMPLM_DECL(false_sharing_impl);

CALLBACK_DECL(increment_nosync);
CALLBACK_DECL(increment_locks);
CALLBACK_DECL(increment_atomic);

// --- FUNCTION DEFINITIONS --- //

void false_sharing(
    char** flags, uint32_t flag_count)
{
    Options options = {
        .job_count      = 1,
        .which_method   = NO_SYNC,
        .data_path      = "",
        .tries          = 1,
        .incr_times     = 0,
    };
    read_flags(
            flags, flag_count,
            &options);

    // If the user wants to use stdout to output data, we suppress any messages to
    // allow only the output that would normally be logged to show up
    if ( strcmp(options.data_path, "") != 0 ) 
    {
        printf("\x1b[33mParallel Systems Postgrad Course -- Project 1 -- Christoforos-Marios Mamaloukas\n\n"
                "---- EXERCISE 3 (False sharing) ----\n"
                "* You selected the version that uses %s\n"
                "* You want %u jobs\n"
                "* Each thread will update %d times\n"
                "* Data will be saved to %s\n"
                "* The experiment will be run %d tries\n"
                "------------------------------------\n\n\x1b[0m", 
                exercise_type[options.which_method],
                options.job_count, 
                options.incr_times,
                options.data_path,
                options.tries);
    }

    false_sharing_impl(&options);

    return;
}

FLAG_READER(options_p)
{
    SET_FLAG("-flock", options_p->which_method, MUTEX);
    SET_FLAG("-fl", options_p->which_method, MUTEX);

    SET_FLAG("-fatom", options_p->which_method, ATOMIC);
    SET_FLAG("-fa", options_p->which_method, ATOMIC);

    SET_FLAG("-fno_sync", options_p->which_method, NO_SYNC);
    SET_FLAG("-fns", options_p->which_method, NO_SYNC);

    SET_FLAG_WITH_NUM("-fincr=", options_p->incr_times, i);
    SET_FLAG_WITH_NUM("-fi=", options_p->incr_times, i);

    END_FLAG_READER();
}

static void false_sharing_impl(const Options* options_p)
{
    if (options_p->which_method == MUTEX)
    {
        incremented_gs = calloc(options_p->job_count, sizeof(uint32_t)); 
    } else if (options_p->which_method == ATOMIC) 
    {
        incremented_atom_gs = calloc(options_p->job_count, sizeof(_Atomic uint32_t)); 
    }

    pthread_t* threads = calloc(
            options_p->job_count,
            sizeof(pthread_t));
    
    if (options_p->which_method == MUTEX)
    {
        pthread_mutex_init(
                &shared_var_mtx,
                NULL);
    }
    
    uint64_t avg_time = 0;
    typedef void* (*thread_callback)(void*);
    thread_callback callbacks[] = {
        &increment_nosync_callback,
        &increment_locks_callback,
        &increment_atomic_callback,
    };
    // we avoid multithreading the loop in this scenario
    for (uint32_t j = 0; j < options_p->tries; j++) 
    {
        for (uint32_t i = 0; i < options_p->job_count; i++)
        {
            uint32_t err = pthread_create(
                    &threads[i],
                    NULL,
                    callbacks[options_p->which_method],
                    (void*)(i));
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

    if (options_p->which_method == MUTEX)
    {
        pthread_mutex_destroy(&shared_var_mtx);
    }
    
    avg_time      /= options_p->tries;

    LOG(
            "[EXERCISE 3]\ntype = %s\njobs = %d\nloops = %d\ntime = %f\n",
            exercise_type[options_p->which_method],
            options_p->job_count,
            options_p->incr_times,
            (double)avg_time/(double)nsec_to_msec_factor);

    if (options_p->which_method == MUTEX)
    {
        free(incremented_gs);
    } else if (options_p->which_method == ATOMIC) 
    {
        free(incremented_atom_gs);
    }
} 

static void* increment_nosync_callback(void* args)
{
    uint32_t threadnum = (uint32_t)args;
    incremented_gs[threadnum]++;
}

static void* increment_locks_callback(void* args)
{
    uint32_t threadnum = (uint32_t)args;

    pthread_mutex_lock(&shared_var_mtx);
    incremented_gs[threadnum]++;
    pthread_mutex_unlock(&shared_var_mtx);

    return NULL;
}

static void* increment_atomic_callback(void* args)
{
    uint32_t threadnum = (uint32_t)args;

    uint64_t intrm = atomic_load(&incremented_atom_gs[threadnum]);
    intrm++;
    atomic_store(&incremented_atom_gs[threadnum], intrm);

    return NULL;
}

