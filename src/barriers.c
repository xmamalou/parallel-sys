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

// --- TYPES --- //

typedef struct Options {
    bool     do_with_busywait;
    bool     do_with_custom;
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

static void read_flags(
    char** flags, uint32_t flag_count,
    Options* options_p);

static void barriers_impl(const Options* options_p);

// args -> the number of iterations to do, cast to uint64_t
static void* barriers_native_callback(void* args);
static void* barriers_custom_callback(void* args);
static void* barriers_custom_busywait_callback(void* args);

// --- FUNCTION DEFINITIONS --- //

void barriers(
    char** flags, uint32_t flag_count)
{
    Options options = {
        .job_count        = 1,
        .do_with_busywait = false,
        .do_with_custom   = false,
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
                exercise_type[
                        !options.do_with_custom 
                                ? 0 
                                : (options.do_with_busywait
                                        ? 2
                                        : 1)],
                options.job_count, 
                options.incr_times,
                options.data_path,
                options.tries);
    }

    barriers_impl(&options);

    return;
}

static void read_flags(
    char** flags, uint32_t flag_count,
    Options* options_p)
{
    for (uint32_t i = 0; i < flag_count; i++)
    {
        if (strcmp(flags[i], "-fcustom") == 0 || strcmp(flags[i], "-fc") == 0)
        {
            options_p->do_with_custom = true;
        }

        if (strcmp(flags[i], "-fbusywait") == 0 || strcmp(flags[i], "-fb") == 0)
        {
            options_p->do_with_custom = true;
            options_p->do_with_busywait = true;
        } 

        if (strcmp(flags[i], "-fpthreads") == 0 || strcmp(flags[i], "-fp") == 0)
        {
            if (options_p->do_with_custom || options_p->do_with_busywait)
            {
                printf("\x1b[31mHey! You requested execution using the pthreads implementation of"
                    "barriers, even though you already selected the custom ones. IGNORING!\n\x1b[0m");
                continue;
            }

            options_p->do_with_custom = false;
        }

        if (strstr(flags[i], "-fjobs=") != NULL || strstr(flags[i], "-fj=") != NULL)
        {
            char* equal_char_p = strchr(flags[i], '=');
            options_p->job_count = atoi(&(equal_char_p[1])); // same as (equal_char_p + sizeof(char)), allows us to get the number next to the `=` sign
        }

        if (strstr(flags[i], "-fn=") != NULL)
        {
            char* equal_char_p = strchr(flags[i], '=');
            options_p->incr_times = atoll(&(equal_char_p[1])); // same as (equal_char_p + sizeof(char)), allows us to get the number next to the `=` sign
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

static void barriers_impl(const Options* options_p)
{
    LOG_T log = NULL;
    if ( strcmp(options_p->data_path, "") != 0 )
    {
        log = open_log(
                options_p->data_path,
                true); 
    }

    pthread_t* threads = calloc(
            options_p->job_count,
            sizeof(pthread_t));
    
    if (!options_p->do_with_custom)
    {
        pthread_barrier_init(
            &shared_var_barr,
            NULL, options_p->job_count);
    } else if (!options_p->do_with_busywait) {
        shared_var_custom_barr = create_barrier(options_p->job_count);
    } else {
        pthread_mutex_init(
                &mutex,
                NULL);
        claimers_g = options_p->job_count;
        cond_g     = false;
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
                    !options_p->do_with_custom
                            ? &barriers_native_callback
                            : (options_p->do_with_busywait
                                    ? &barriers_custom_busywait_callback
                                    : &barriers_custom_callback),
                    (void*)(options_p->incr_times));
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

    if (!options_p->do_with_custom)
    {
        pthread_barrier_destroy(&shared_var_barr);
    } else if (!options_p->do_with_busywait) {
        destroy_barrier(shared_var_custom_barr);
    } else {
        pthread_mutex_destroy(&mutex);
    }
    
    avg_time      /= options_p->tries;

    if ( strcmp(options_p->data_path, "") != 0 )
    {
        printf("Done with the barriers! This took %f msecs!", 
                (double)avg_time/(double)nsec_to_msec_factor);

        char text[PATH_MAX] = "";
        sprintf(
                text,
                "[EXERCISE 4]\ntype = %s\njobs = %d\nloops = %d\ntime = %f\n",
                exercise_type[
                        !options_p->do_with_custom 
                                ? 0 
                                : 1],
                options_p->job_count,
                options_p->incr_times,
                (double)avg_time/(double)nsec_to_msec_factor);
        write_log(
                log,
                text);
        close_log(log);
    } else {
        printf(
                "[EXERCISE 4]\ntype = %s\njobs = %d\nloops\ntime = %f\n",
                exercise_type[
                        !options_p->do_with_custom 
                        ? 0 
                        : 1],
                options_p->job_count,
                options_p->incr_times,
                (double)avg_time/(double)nsec_to_msec_factor);
    } 
} 

static void* barriers_native_callback(void* args)
{
    uint64_t loop_times = (uint64_t)args;

    for (uint64_t i = 0; i < loop_times; i++)
    {
        pthread_barrier_wait(&shared_var_barr);
    }

    return NULL;
}

static void* barriers_custom_callback(void* args)
{
    uint64_t loop_times = (uint64_t)args;

    for (uint64_t i = 0; i < loop_times; i++)
    {
        waitfor_barrier(shared_var_custom_barr);
    }

    return NULL;
}

// TODO: Doesn't work
static void* barriers_custom_busywait_callback(void* args)
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

