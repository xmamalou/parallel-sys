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

// -- TYPES --- //

typedef struct Options {
    uint64_t throw_count;
    uint32_t job_count;
    bool     do_serial;
    bool     do_omp;
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

static void read_flags_ex1(
        char** flags, uint32_t flag_count,
        Options* options_p);

inline static void pi_calc_serial(const Options* options_p);
inline static void pi_calc_parallel(const Options* options_p);
inline static void pi_calc_openmp(const Options* options_p);

/// @brief Function that calculates amount of successful throws
/// @param args Pointer to thread parametrization structure
/// @return NULL
void* succ_throws_callback(void* args);

// --- FUNCTION DEFINITIONS --- //

void pi_calc(
        char** flags, uint32_t flag_count) 
{
    Options options = {
        .job_count   = 1,
        .throw_count = 10, 
        .do_serial   = false,
        .do_omp      = false,
        .data_path   = "",
        .tries       = 1,
    };
    read_flags_ex1(
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
                exercise_type[
                        options.do_serial 
                        ? 0 
                        : (!options.do_omp 
                            ? 1 
                            : 2)],
                options.job_count, 
                options.throw_count,
                options.data_path,
                options.tries);
    }

    if (options.do_serial)
    {
        pi_calc_serial(&options);
    } else if (options.do_omp) {
        pi_calc_openmp(&options);
    } else {
        pi_calc_parallel(&options);
    }

    return;
}

static void read_flags_ex1(
    char** flags, uint32_t flag_count,
    Options* options_p)
{
    for (uint32_t i = 0; i < flag_count; i++)
    {
        if (strcmp(flags[i], "-fs") == 0 || strcmp(flags[i], "-fserial") == 0)
        {
            if (options_p->do_omp)
            {
                printf("\x1b[31mHey! You requested serial execution, even though"
                    " you already want parallel! IGNORING!\n\x1b[0m");
                continue;
            }

            options_p->do_serial = true;
            options_p->do_omp    = false;
        }

        if (strcmp(flags[i], "-fomp") == 0)
        {
            if(options_p->do_serial)
            {
                printf("\x1b[31mHey! You requested OpenMP to be used, even though"
                        " you want serial execution! IGNORING!\n\x1b[0m");
                continue;
            }

            options_p->do_serial = false;
            options_p->do_omp    = true;
        }

        if (strstr(flags[i], "-fjobs=") != NULL || strstr(flags[i], "-fj=") != NULL)
        {
            if(options_p->do_serial)
            {
                printf("\x1b[31mHey! You requested a custom amount of jobs, even though"
                        " you want serial execution! IGNORING!\n\x1b[0m");
                continue;
            }

            char* equal_char_p = strchr(flags[i], '=');
            options_p->job_count = atoi(&(equal_char_p[1])); // same as (equal_char_p + sizeof(char)), allows us to get the number next to the `=` sign
        }

        if (strstr(flags[i], "-fthrows=") != NULL || strstr(flags[i], "-fn=") != NULL)
        {
            char* equal_char_p     = strchr(flags[i], '=');
            options_p->throw_count = atoll(&(equal_char_p[1])); // same as (equal_char_p + sizeof(char)), allows us to get the number next to the `=` sign
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

inline static void pi_calc_serial(const Options* options_p) 
{
    LOG_T log = NULL;
    if ( strcmp(options_p->data_path, "") != 0 )
    {
        printf("I estimate that π is approximately equal to... ");
        log = open_log(
                options_p->data_path,
                true); 
    }

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

    if ( strcmp(options_p->data_path, "") != 0 )
    {
        char text[PATH_MAX] = "";
        printf("%f!\nThis took %f msecs!", 
                pi_val, 
                (double)avg_time/(double)nsec_to_msec_factor);
        
        sprintf(
                text,
                "[EXERCISE 1]\ntype = serial\nthrows = %d\nπ = %f\ntime = %f\n",
                options_p->throw_count, 
                pi_val, 
                (double)avg_time/(double)nsec_to_msec_factor);
        write_log(
                log,
                text);
        close_log(log);
    } else {
        printf("[EXERCISE 1]\ntype = serial\nthrows = %d\nπ = %f\ntime = %f\n", 
            options_p->throw_count,
            pi_val, 
            (double)avg_time/(double)nsec_to_msec_factor);
    }
}

inline static void pi_calc_parallel(const Options* options_p) 
{
    LOG_T log = NULL;
    if ( strcmp(options_p->data_path, "") != 0 )
    {
        printf("I estimate that π is approximately equal to... ");

        log = open_log(
                options_p->data_path,
                true); 
    }
    
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
        }
        avg_time += stop_benchmark(bench_h);
    }
    
    avg_time      /= options_p->tries;
    succ_throws_g /= options_p->tries;
    float pi_val   = 4*succ_throws_g/(double)options_p->throw_count;

    if ( strcmp(options_p->data_path, "") != 0 )
    {
        printf("%f!\nThis took %f msecs!", 
                pi_val, 
                (double)avg_time/(double)nsec_to_msec_factor);

        char text[PATH_MAX] = "";
        sprintf(
                text,
                "[EXERCISE 1]\ntype = pthreads\njobs = %d\nthrows = %d\nπ = %f\ntime = %f\n",
                options_p->job_count,
                options_p->throw_count,
                pi_val, 
                (double)avg_time/(double)nsec_to_msec_factor);
        write_log(
                log,
                text);
        close_log(log);
    } else {
        printf(
                "[EXERCISE 1]\ntype = pthreads\njobs = %d\nthrows = %d\nπ = %f\ntime = %f\n",
                options_p->job_count,
                options_p->throw_count,
                pi_val, 
                (double)avg_time/(double)nsec_to_msec_factor);
    } 

}

inline static void pi_calc_openmp(const Options* options_p)
{
    LOG_T log = NULL;
    if ( strcmp(options_p->data_path, "") != 0 )
    {
        printf("I estimate that π is approximately equal to... ");

        log = open_log(
                options_p->data_path,
                true); 
    }
    
    uint64_t throws_per_job  = options_p->throw_count/options_p->job_count;
    uint64_t avg_succ_throws = 0;
    uint64_t avg_time        = 0;
    
    for (uint32_t j = 0; j < options_p->tries; j++)
    {
        BENCHMARK_T bench_h = start_benchmark();
        #pragma omp parallel for num_threads(job_count) \
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
    if ( strcmp(options_p->data_path, "") != 0 ) 
    {
        printf("%f!\nThis took %f msecs!", 
                pi_val, 
                (double)avg_time/(double)nsec_to_msec_factor);

        char text[PATH_MAX] = "";
        sprintf(
                text,
                "[EXERCISE 1]\ntype = openmp\njobs = %d\nthrows = %d\nπ = %f\ntime = %f\n", 
                options_p->job_count, 
                options_p->throw_count,
                pi_val, 
                (double)avg_time/(double)nsec_to_msec_factor);
        write_log(
                log,
                text);
        close_log(log);
    } else {
        printf(
            "[EXERCISE 1]\ntype = openmp\njobs = %d\nthrows = %d\nπ = %f\ntime = %f\n", 
            options_p->job_count, 
            options_p->throw_count,
            pi_val, 
            (double)avg_time/(double)nsec_to_msec_factor);
    }
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