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
#include <assert.h>

#include "utility.h"
#include "macros.h"

// --- TYPES --- //

typedef struct Options {
    bool     do_with_atomic;
    uint32_t tries;
    uint32_t job_count;
    uint64_t loops;
    char     data_path[PATH_MAX];
} Options;

// --- CONSTANTS --- //

static const char* exercise_type[] = {
    "locks",
    "atomics",
};

static const uint64_t nsec_to_sec_factor  = 1000000000;
static const uint64_t nsec_to_msec_factor = 1000000;

// --- GLOBALS --- //

static uint64_t         incremented_g = 0;
static _Atomic uint64_t incremented_atom_g = 0;
static pthread_mutex_t  shared_var_mtx;

// --- FUNCTION DECLARATIONS --- //

FLAG_READER_DECL();

EXERCISE_IMPLM_DECL(shared_var_impl);

CALLBACK_DECL(increment_locks);
CALLBACK_DECL(increment_atomic);

// --- FUNCTION DEFINITIONS --- //

void shared_var(
    char** flags, uint32_t flag_count)
{
    Options options = {
        .job_count      = 1,
        .loops          = 1,
        .do_with_atomic = false,
        .data_path      = "",
        .tries          = 1,
    };
    read_flags(
            flags, flag_count,
            &options);

    // If the user wants to use stdout to output data, we suppress any messages to
    // allow only the output that would normally be logged to show up
    if ( strcmp(options.data_path, "") != 0 ) 
    {
        printf("\x1b[33mParallel Systems Postgrad Course -- Project 1 -- Christoforos-Marios Mamaloukas\n\n"
                "---- EXERCISE 2 (Shared Variable) ----\n"
                "* You selected the version that uses %s\n"
                "* You want %u jobs\n"
                "* The variable will be incremented %lu times\n"
                "* Data will be saved to %s\n"
                "* The experiment will be run %d tries\n"
                "--------------------------------------\n\n\x1b[0m", 
                exercise_type[
                        !options.do_with_atomic 
                        ? 0 
                        : 1],
                options.job_count, 
                options.loops,
                options.data_path,
                options.tries);
    }

    double time_of_execution = 0;
    shared_var_impl(
            &options,
            &time_of_execution);

    CALCULATE_TIME(time_of_execution);

    LOG(
            "[EXERCISE 2]\ntype = %s\njobs = %d\nloops = %lu\ntime = %f\n",
            exercise_type[
                !options.do_with_atomic 
                ? 0 
                : 1],
            options.job_count,
            options.loops,
            time_of_execution); 

    return;
}

FLAG_READER(options_p)
{
    SET_FLAG("-flock", options_p->do_with_atomic, false);
    SET_FLAG("-fl", options_p->do_with_atomic, false);

    SET_FLAG("-fatom", options_p->do_with_atomic, true);
    SET_FLAG("-fa", options_p->do_with_atomic, true);

    SET_FLAG_WITH_NUM("-fincr=", options_p->loops, ll);
    SET_FLAG_WITH_NUM("-fi=", options_p->loops, ll);

    END_FLAG_READER();
}

EXERCISE_IMPLM_DECL(shared_var_impl)
{
    pthread_t* threads = calloc(
            options_p->job_count,
            sizeof(pthread_t));
    
    if (!options_p->do_with_atomic)
    {
        pthread_mutex_init(
                &shared_var_mtx,
                NULL);
    }
    
    // we avoid multithreading the loop in this scenario
    for (uint32_t j = 0; j < options_p->tries; j++) 
    {
        BENCHMARK_T bench_h = start_benchmark();
        for (uint32_t i = 0; i < options_p->job_count; i++)
        {
            uint32_t err = pthread_create(
                    &threads[i],
                    NULL,
                    !options_p->do_with_atomic 
                            ? &increment_locks_callback
                            : &increment_atomic_callback,
                    (void*)(options_p->loops));
        }

        // now we wait for all the threads to finish
        for (uint32_t i = 0; i < options_p->job_count; i++)
        {
            pthread_join(
                    threads[i],
                    NULL);
            threads[i] == NULL;
        }

        assert(
                incremented_g == options_p->loops*options_p->job_count
                || incremented_atom_g == options_p->loops*options_p->job_count);

        RECORD(bench_h);
    }

    free(threads);

    if (!options_p->do_with_atomic)
    {
        pthread_mutex_destroy(&shared_var_mtx);
    }
    
} 

CALLBACK_DECL(increment_locks)
{
    uint64_t loops = (uint64_t)args;

    pthread_mutex_lock(&shared_var_mtx);
    for (uint64_t i = 0; i < loops; i++)
    {   
        incremented_g++;
    }
    pthread_mutex_unlock(&shared_var_mtx);

    return NULL;
}

CALLBACK_DECL(increment_atomic)
{
    uint64_t loops = (uint64_t)args;

    for (uint64_t i = 0; i < loops; i++)
    {
        uint64_t intrm = atomic_load(&incremented_atom_g);
        intrm++;
        atomic_store(&incremented_atom_g, intrm);
    }
   

    return NULL;
}

