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

#include "utility.h"

// -- TYPES --- //

typedef struct PiCalcOptions {
    uint64_t throw_count;
    uint32_t job_count;
    bool     do_serial;
    bool     do_omp;
} PiCalcOptions;

// --- CONSTANTS --- //

const char* exercise_type[] = {
    "serial",
    "parallel with pthreads",
    "parallel with OpenMP",
};

const uint64_t nsec_to_sec_factor  = 1000000000;
const uint64_t nsec_to_msec_factor = 1000000;


// --- FUNCTION DECLARATIONS --- //

void read_flags_ex1(
        char** flags, uint32_t flag_count,
        PiCalcOptions* options_p);

inline void pi_calc_serial(uint64_t throw_count);
inline void pi_calc_parallel(
        uint32_t job_count,
        uint32_t throw_count);
inline void pi_calc_openmp( 
        uint32_t job_count,
        uint32_t throw_count);

// --- FUNCTION DEFINITIONS --- //

void pi_calc(
        char** flags, uint32_t flag_count) 
{
    PiCalcOptions options = {
        .job_count   = 1,
        .throw_count = 10, 
        .do_serial   = false,
        .do_omp      = false,
    };
    read_flags_ex1(
            flags, flag_count,
            &options);

    printf("\x1b[33mParallel Systems Postgrad Course -- Project 1 -- Christoforos-Marios Mamaloukas\n\n"
            "--- EXERCISE 1 (Calculation of π) ---\n"
            "* You selected %s execution\n"
            "* You want %u jobs\n"
            "* You want %lu throws\n"
            "-------------------------------------\n\n\x1b[0m", 
            exercise_type[
                    options.do_serial 
                    ? 0 
                    : (!options.do_omp 
                        ? 1 
                        : 2)],
            options.job_count, 
            options.throw_count);

    if (options.do_serial)
    {
        pi_calc_serial(options.throw_count);
    } else if (options.do_omp) {
        pi_calc_openmp(
                options.job_count,
                options.throw_count);
    } else {
        pi_calc_parallel(
                options.job_count,
                options.throw_count);
    }

    return;
}

void read_flags_ex1(
    char** flags, uint32_t flag_count,
    PiCalcOptions* options_p)
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
    }
}

inline void pi_calc_serial(uint64_t throw_count) 
{
    printf("I estimate that π is approximately equal to... ");

    // throws inside the circle
    uint64_t succ_throws = 0;

    BenchmarkHandle bench_h = start_benchmark();
    for (uint64_t i = 0; i < throw_count; i++)
    {
        double x = (double)rand()/(double)RAND_MAX, 
                y = (double)rand()/(double)RAND_MAX;

        if (x*x + y*y <= 1.0) 
        {
            succ_throws++;
        }
    }
    uint64_t time = stop_benchmark(bench_h);

    printf("%f!\nThis took %f msecs!", 
            4*succ_throws/(double)throw_count, 
            (double)time/(double)nsec_to_msec_factor);
}

inline void pi_calc_parallel(
    uint32_t job_count,
    uint32_t throw_count) 
{

}

inline void pi_calc_openmp( 
    uint32_t job_count,
    uint32_t throw_count)
{

}