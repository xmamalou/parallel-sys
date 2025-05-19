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

#define __USE_XOPEN2K

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
    PTHREADS = 0,
    CUSTOM   = 1,
    BUSYWAIT = 2,
} Which;

typedef struct Options {
    Which    which_implm;
    uint32_t tries;
    uint32_t job_count;
    uint64_t incr_times;
    char     data_path[PATH_MAX];
} Options;

// --- CONSTANTS --- //

static const char* exercise_type[] = {
    "pthreads",
    "custom with condition variable",
    "custom with busy waiting"
};

static const uint64_t nsec_to_sec_factor  = 1000000000;
static const uint64_t nsec_to_msec_factor = 1000000;

// --- GLOBALS --- //

static pthread_barrier_t shared_var_barr;
static BARRIER_T         shared_var_custom_barr;

static pthread_mutex_t   mutex;
static bool              cond_g;
static uint32_t          claimers_g;
static uint32_t          current_claimers_g;

// --- FUNCTION DECLARATIONS --- //

FLAG_READER_DECL();

EXERCISE_IMPLM_DECL(barriers_impl);

CALLBACK_DECL(barriers_native);
CALLBACK_DECL(barriers_custom);
CALLBACK_DECL(barriers_custom_busywait);

// --- FUNCTION DEFINITIONS --- //

void barriers(
    char** flags, uint32_t flag_count)
{
    Options options = {
        .job_count        = 1,
        .which_implm      = PTHREADS,
        .data_path        = "",
        .tries            = 1,
        .incr_times       = 1,
    };
    read_flags(
            flags, flag_count,
            &options);

    // If the user wants to use stdout to output data, we suppress any messages to
    // allow only the output that would normally be logged to show up
    if ( strcmp(options.data_path, "") != 0 ) 
    {
        printf("\x1b[33mParallel Systems Postgrad Course -- Project 1 -- Christoforos-Marios Mamaloukas\n\n"
                "---- EXERCISE 4 (Barriers) ----\n"
                "* You selected the version that uses the %s implementation\n"
                "* You want %u jobs\n"
                "* Each thread will loop %d times\n"
                "* Data will be saved to %s\n"
                "* The experiment will be run %d tries\n"
                "------------------------------------\n\n\x1b[0m", 
                exercise_type[options.which_implm],
                options.job_count, 
                options.incr_times,
                options.data_path,
                options.tries);
    }

    double time_of_execution = 0;
    barriers_impl(
            &options,
            &time_of_execution);

    CALCULATE_TIME(time_of_execution);

    LOG(
        "[EXERCISE 4]\ntype = %s\njobs = %d\nloops = %d\ntime = %f\n",
        exercise_type[options.which_implm],
        options.job_count,
        options.incr_times,
        time_of_execution);

    return;
}

FLAG_READER(options_p)
{

        // Which implementation to use
        SET_FLAG("-fcustom", options_p->which_implm, CUSTOM);
        SET_FLAG("-fc", options_p->which_implm, CUSTOM);

        SET_FLAG("-fbusywait", options_p->which_implm, BUSYWAIT);
        SET_FLAG("-fb", options_p->which_implm, BUSYWAIT);

        SET_FLAG("-fpthreads", options_p->which_implm, PTHREADS);
        SET_FLAG("-fp", options_p->which_implm, PTHREADS);
        //

        // how many times
        SET_FLAG_WITH_NUM("-fn=", options_p->incr_times, ll);
        //

        END_FLAG_READER();
}

EXERCISE_IMPLM_DECL(barriers_impl)
{
    pthread_t* threads = calloc(
            options_p->job_count,
            sizeof(pthread_t));
    
    switch (options_p->which_implm)
    {
    case BUSYWAIT:
        pthread_mutex_init(
                &mutex,
                NULL);
        claimers_g = options_p->job_count;
        cond_g     = false;
        break;
    case CUSTOM:
        shared_var_custom_barr = create_barrier(options_p->job_count);
        break;
    
    case PTHREADS:
    default:
        pthread_barrier_init(
                &shared_var_barr,
                NULL, options_p->job_count);
        break;
    }
    
    CALLBACK_T callbacks[] = {
        &barriers_native_callback,
        &barriers_custom_callback,
        &barriers_custom_busywait_callback,
    };
    // we avoid multithreading the loop in this scenario
    for (uint32_t j = 0; j < options_p->tries; j++) 
    {
        BENCHMARK_T bench_h = start_benchmark();
        for (uint32_t i = 0; i < options_p->job_count; i++)
        {
            uint32_t err = pthread_create(
                    &threads[i],
                    NULL,
                    callbacks[options_p->which_implm],
                    (void*)(options_p->incr_times));
        }

        // now we wait for all the threads to finish
        for (uint32_t i = 0; i < options_p->job_count; i++)
        {
            pthread_join(
                    threads[i],
                    NULL);
            threads[i] == NULL;
        }
        RECORD(bench_h);
    }

    // TODO: This breaks if threads are more than 4 or 5 or so, because God hates me
    free(threads);

    switch (options_p->which_implm)
    {
    case BUSYWAIT:
        pthread_mutex_destroy(&mutex);
        break;
    case CUSTOM:
        destroy_barrier(shared_var_custom_barr);
        break;
    case PTHREADS:
    default:
        pthread_barrier_destroy(&shared_var_barr);
        break;
    }
} 

CALLBACK_DECL(barriers_native)
{
    uint64_t loop_times = (uint64_t)args;

    for (uint64_t i = 0; i < loop_times; i++)
    {
        pthread_barrier_wait(&shared_var_barr);
    }

    return NULL;
}

CALLBACK_DECL(barriers_custom)
{
    uint64_t loop_times = (uint64_t)args;

    for (uint64_t i = 0; i < loop_times; i++)
    {
        waitfor_barrier(shared_var_custom_barr);
    }

    return NULL;
}

// This was magically fixed because ????????
CALLBACK_DECL(barriers_custom_busywait)
{
    uint64_t loop_times = (uint64_t)args;

    for (uint64_t i = 0; i < loop_times; i++)
    {
        pthread_mutex_lock(&mutex);
        current_claimers_g++;
        pthread_mutex_unlock(&mutex);

        if (current_claimers_g == claimers_g)
        {
            current_claimers_g = 0;
            cond_g = true;
        } else {
            while (!cond_g) printf("");
        }
    }

    return NULL;
}

