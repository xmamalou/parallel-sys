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
#include <assert.h>

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
    uint16_t* data;
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

void merge_sort_algo(
    uint16_t* array, uint64_t left, uint64_t right);
void merge(
    uint16_t* array, uint64_t left, uint64_t middle, uint64_t right);

void merge_sort_algo_parallel(
    uint16_t* array, uint64_t left, uint64_t right);

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

    options.data = calloc(
            options.element_count,
            sizeof(uint16_t));
    FILE* urandom = fopen("/dev/urandom", "r");
    for (uint32_t i = 0; i < options.element_count; i++)
    {
        uint16_t rand_num = 0;
        fread(&rand_num, sizeof(uint16_t), 1, urandom);
        options.data[i] = (uint16_t)rand_num;
    }
    fclose(urandom);

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

    free(options.data);

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
    for (uint32_t i = 0; i < options_p->tries; i++)
    {
        BENCHMARK_T benchmark = start_benchmark();

        merge_sort_algo(
                options_p->data,
                0,
                options_p->element_count - 1);

        RECORD(benchmark);

        for (uint64_t j = 0; j < options_p->element_count - 1; j++)
        {
            assert(
                    options_p->data[j] <= options_p->data[j + 1]);
        }
    }
} 

EXERCISE_IMPLM_DECL(merge_sort_parallel)
{
    for (uint32_t i = 0; i < options_p->tries; i++)
    {
        BENCHMARK_T benchmark = start_benchmark();

        #pragma omp parallel
        {
            #pragma omp single
            merge_sort_algo_parallel(
                options_p->data,
                0,
                options_p->element_count - 1);
        }

        RECORD(benchmark);

        for (uint64_t j = 0; j < options_p->element_count - 1; j++)
        {
            assert(
                    options_p->data[j] <= options_p->data[j + 1]);
        }
    }
}

void merge_sort_algo(
    uint16_t* array, uint64_t left, uint64_t right)
{
    if (left < right)
    {
        uint64_t middle = left + (right - left) / 2;

        merge_sort_algo(array, left, middle);
        merge_sort_algo(array, middle + 1, right);

        merge(array, left, middle, right);
    }
}

void merge(
    uint16_t* array, uint64_t left, uint64_t middle, uint64_t right)
{
    uint64_t i, j, k;
    uint64_t n1 = middle - left + 1;
    uint64_t n2 = right - middle;

    uint16_t left_arr[n1], right_arr[n2];

    for (i = 0; i < n1; i++)
    {
        left_arr[i] = array[left + i];
    }
    for (j = 0; j < n2; j++)
    {
        right_arr[j] = array[middle + 1 + j];
    }

    
    i = 0;
    j = 0;
    k = left;
    while (i < n1 && j < n2)
    {
        if (left_arr[i] <= right_arr[j])
        {
            array[k++] = left_arr[i++];
        } else {
            array[k++] = right_arr[j++];
        }
    }

    while (i < n1)
    {
        array[k++] = left_arr[i++];
    }

    while (j < n2)
    {
        array[k++] = right_arr[j++];
    }
}

void merge_sort_algo_parallel(
    uint16_t* array, uint64_t left, uint64_t right)
{
    if (left < right)
    {
        uint64_t middle = left + (right - left) / 2;

        #pragma omp task shared(array) 
        if(right - left > 1000)
        {
            merge_sort_algo_parallel(array, left, middle);
        }
        
        #pragma omp task shared(array) 
        if(right - left > 1000)
        {
            merge_sort_algo_parallel(array, middle + 1, right);
        }

        #pragma omp taskwait
        merge(array, left, middle, right);
    }
}
