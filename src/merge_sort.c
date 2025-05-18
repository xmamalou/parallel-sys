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
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#include <omp.h>

#include "utility.h"
#include "macros.h"

// --- TYPES --- //

typedef enum Which {
    SERIAL   = 0,
    PARALLEL = 1,
} Which;

typedef struct Options {
    Which     which_implm;
    uint32_t  tries;
    uint32_t  job_count;
    uint64_t  element_count;
    uint64_t* data;
    char      data_path[PATH_MAX];
} Options;

// --- CONSTANTS --- //

static const char* exercise_type[] = {
    "serial",
    "parallel",
};

static const uint64_t nsec_to_sec_factor  = 1000000000;
static const uint64_t nsec_to_msec_factor = 1000000;

// --- FUNCTION DECLARATIONS --- //

FLAG_READER_DECL();

EXERCISE_IMPLM_DECL(merge_sort_serial);
EXERCISE_IMPLM_DECL(merge_sort_parallel);

// --- FUNCTION DEFINITIONS --- //

void merge_sort(
    char** flags, uint32_t flag_count)
{
    Options options = {
        .job_count        = 1,
        .which_implm      = SERIAL,
        .data_path        = "",
        .tries            = 1,
        .element_count    = 1,
        .data             = NULL,
    };
    read_flags(
            flags, flag_count,
            &options);

    // If the user wants to use stdout to output data, we suppress any messages to
    // allow only the output that would normally be logged to show up
    if ( strcmp(options.data_path, "") != 0 ) 
    {
        printf("\x1b[33mParallel Systems Postgrad Course -- Project 1 -- Christoforos-Marios Mamaloukas\n\n"
                "---- EXERCISE 8 (Merge sort) ----\n"
                "* You selected the %s implementation\n"
                "* You want %u jobs\n"
                "* The array will have %d elements\n"
                "* Data will be saved to %s\n"
                "* The experiment will be run %d tries\n"
                "------------------------------------\n\n\x1b[0m", 
                exercise_type[options.which_implm],
                options.job_count, 
                options.element_count,
                options.data_path,
                options.tries);
    }

    double time_of_execution = 0;
    EXERCISE_IMPLM_T functions[] = {
        &merge_sort_serial,
        &merge_sort_parallel,
    };
    functions[options.which_implm](
            &options,
            &time_of_execution);

    CALCULATE_TIME(time_of_execution);

    LOG(
        "[EXERCISE 8]\ntype = %s\njobs = %d\nelements = %d\ntime = %f\n",
        exercise_type[options.which_implm],
        options.job_count,
        options.element_count,
        time_of_execution);

    return;
}

FLAG_READER(options_p)
{

        // Which implementation to use
        SET_FLAG("-fserial", options_p->which_implm, SERIAL);
        SET_FLAG("-fs", options_p->which_implm, SERIAL);

        SET_FLAG("-fparallel", options_p->which_implm, PARALLEL);
        SET_FLAG("-fp", options_p->which_implm, PARALLEL);
        //

        // how many times
        SET_FLAG_WITH_NUM("-fn=", options_p->element_count, ll);
        //

        END_FLAG_READER();
}

EXERCISE_IMPLM_DECL(merge_sort_serial)
{
    
} 

EXERCISE_IMPLM_DECL(merge_sort_parallel)
{
}

