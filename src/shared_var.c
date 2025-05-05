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

// --- TYPES --- //

typedef struct Options {
    bool     do_with_atomic;
    uint32_t tries;
    uint32_t job_count;
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

static void read_flags(
    char** flags, uint32_t flag_count,
    Options* options_p);

static void shared_var_impl(const Options* options_p);

static void* increment_locks_callback(void* args);
static void* increment_atomic_callback(void* args);

// --- FUNCTION DEFINITIONS --- //

void shared_var(
    char** flags, uint32_t flag_count)
{
    Options options = {
        .job_count      = 1,
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
                "* Data will be saved to %s\n"
                "* The experiment will be run %d tries\n"
                "--------------------------------------\n\n\x1b[0m", 
                exercise_type[
                        !options.do_with_atomic 
                        ? 0 
                        : 1],
                options.job_count, 
                options.data_path,
                options.tries);
    }

    shared_var_impl(&options);

    return;
}

static void read_flags(
    char** flags, uint32_t flag_count,
    Options* options_p)
{
    for (uint32_t i = 0; i < flag_count; i++)
    {
        if (strcmp(flags[i], "-flock") == 0 || strcmp(flags[i], "-fl") == 0)
        {
            if (options_p->do_with_atomic)
            {
                fprintf(stderr,
                        "\x1b[31mHey! You requested execution using locks, even though"
                        " you already want it using atomics! IGNORING!\n\x1b[0m");
                continue;
            }

            options_p->do_with_atomic = false;
        }

        if (strcmp(flags[i], "-fatom") == 0 || strcmp(flags[i], "-fa") == 0)
        {
            options_p->do_with_atomic = true;
        }

        if (strstr(flags[i], "-fjobs=") != NULL || strstr(flags[i], "-fj=") != NULL)
        {
            char* equal_char_p = strchr(flags[i], '=');
            options_p->job_count = atoi(&(equal_char_p[1])); // same as (equal_char_p + sizeof(char)), allows us to get the number next to the `=` sign
        }

        if (strstr(flags[i], "-ffile=") != NULL || strstr(flags[i], "-ff=") != NULL)
        {
            char* equal_char_p     = strchr(flags[i], '=');
            if (equal_char_p[1] != '~' || equal_char_p[1] != '/')
            {
                getcwd(options_p->data_path, PATH_MAX);
                strcat(options_p->data_path, "/");
            }
            strncat(
                    options_p->data_path,
                    &equal_char_p[1],
                    strlen(&equal_char_p[1]));
        }

        if (strstr(flags[i], "-ftries=") != NULL || strstr(flags[i], "-ft=") != NULL)
        {
            char* equal_char_p = strchr(flags[i], '=');
            options_p->tries = atoi(&(equal_char_p[1])); // same as (equal_char_p + sizeof(char)), allows us to get the number next to the `=` sign
        }
    }
}

static void shared_var_impl(const Options* options_p)
{
    LOG_T log = NULL;
    if ( strcmp(options_p->data_path, "") != 0 )
    {
        printf("Working on that variable... ");

        log = open_log(
                options_p->data_path,
                true); 
    }
    
    pthread_t* threads = calloc(
            options_p->job_count,
            sizeof(pthread_t));
    
    if (!options_p->do_with_atomic)
    {
        pthread_mutex_init(
                &shared_var_mtx,
                NULL);
    }
    
    uint64_t avg_time = 0;
    // we avoid multithreading the loop in this scenario
    for (uint32_t j = 0; j < options_p->tries; j++) 
    {
        for (uint32_t i = 0; i < options_p->job_count; i++)
        {
            uint32_t err = pthread_create(
                    &threads[i],
                    NULL,
                    !options_p->do_with_atomic 
                            ? &increment_locks_callback
                            : &increment_atomic_callback,
                    NULL);
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

    if (!options_p->do_with_atomic)
    {
        pthread_mutex_destroy(&shared_var_mtx);
    }
    
    avg_time      /= options_p->tries;

    if ( strcmp(options_p->data_path, "") != 0 )
    {
        printf("DONE! The result is %d! This took %f msecs!", 
                !options_p->do_with_atomic
                        ? incremented_g/options_p->tries
                        : atomic_load(&incremented_atom_g)/options_p->tries, 
                (double)avg_time/(double)nsec_to_msec_factor);

        char text[PATH_MAX] = "";
        sprintf(
                text,
                "[EXERCISE 2]\ntype = %s\njobs = %d\ntime = %f\n",
                exercise_type[
                    !options_p->do_with_atomic 
                    ? 0 
                    : 1],
                options_p->job_count,
                (double)avg_time/(double)nsec_to_msec_factor);
        write_log(
                log,
                text);
        close_log(log);
    } else {
        printf(
                "[EXERCISE 2]\ntype = %s\njobs = %d\ntime = %f\n",
                exercise_type[
                    !options_p->do_with_atomic 
                    ? 0 
                    : 1],
                options_p->job_count,
                (double)avg_time/(double)nsec_to_msec_factor);
    } 
} 

static void* increment_locks_callback(void* args)
{
    pthread_mutex_lock(&shared_var_mtx);
    incremented_g++;
    pthread_mutex_unlock(&shared_var_mtx);

    return NULL;
}

static void* increment_atomic_callback(void* args)
{
    uint64_t intrm = atomic_load(&incremented_atom_g);
    intrm++;
    atomic_store(&incremented_atom_g, intrm);

    return NULL;
}

