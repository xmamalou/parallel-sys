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

#define _GNU_SOURCE
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
    UNOPTIMIZED = 0,
    OPTIMIZED   = 1,
} Which;

typedef struct Options {
    uint32_t job_count;
    Which    which_implm;
    char     matrix_dims[PATH_MAX];
    uint64_t columns;
    uint64_t rows;
    bool     is_upper_triangular;
    char     data_path[PATH_MAX];
    uint32_t tries;
} Options;

// --- CONSTANTS --- //

#define BIG_BUFF 256

static const char* implm_string[] = {
    "unoptimized",
    "optimized",
};

static const uint64_t nsec_to_sec_factor  = 1000000000;
static const uint64_t nsec_to_msec_factor = 1000000;

// --- FUNCTION DECLARATIONS --- //

FLAG_READER_DECL();

/// @brief The original unoptimized version of the algorithm
EXERCISE_IMPLM_DECL(better_mul_unopt);
/// @brief The optimized version of the algorithm for upper triangular matrices
EXERCISE_IMPLM_DECL(better_mul_opt);

// --- FUNCTION DEFINITIONS --- //

void better_mul(
        char** flags, uint32_t flag_count) 
{
    Options options = {
        .job_count           = 1,
        .which_implm         = OPTIMIZED,
        .matrix_dims         = "",
        .columns             = 2,
        .rows                = 2,
        .is_upper_triangular = false,
        .data_path           = "",
        .tries               = 1,
    };
    read_flags(
            flags, flag_count,
            &options);

    // If the user wants to use stdout to output data, we suppress any messages to
    // allow only the output that would normally be logged to show up
    if ( strcmp(options.data_path, "") != 0 ) 
    {
        printf("\x1b[33mParallel Systems Postgrad Course -- Project 1 -- Christoforos-Marios Mamaloukas\n\n"
                "--- EXERCISE 5 (Better matrix multiplication) ---\n"
                "* You will run the %s version\n"
                "* You want %u jobs\n"
                "* The matrix will have dimensions %s\n"
                "* The matrix is %supper triangular\n"
                "* Data will be saved to %s\n"
                "* The experiment will be run %d tries\n"
                "-------------------------------------------------\n\n\x1b[0m",
                implm_string[options.which_implm],
                options.job_count, 
                options.matrix_dims,
                options.is_upper_triangular ? "" : "not ",
                options.data_path,
                options.tries);
    }

    sscanf(
        options.matrix_dims,
        "%ux%u",
        &options.columns,
        &options.rows);
    if (options.columns != options.rows && options.is_upper_triangular)
    {
        ERROR(
            "Hey! You requested an upper triangular matrix," 
            "but the dimensions you provided (%s) are not square! IGNORING ROWS!", 
            options.matrix_dims);
        options.rows = options.columns;
    }

    double time_of_execution = 0;

    EXERCISE_IMPLM_T functions[] = {
        &better_mul_unopt,
        &better_mul_opt,
    };
    functions[options.which_implm](
            &options,
            &time_of_execution);

    CALCULATE_TIME(time_of_execution);

    LOG(
        "[EXERCISE 5]\ntype = %s\njobs = %d\ntime = %f\n",
        implm_string[options.which_implm],
        options.job_count,
        time_of_execution);

    return;
}

FLAG_READER(options_p)
{
    // Which implementation to use
    SET_FLAG("-fO0", options_p->which_implm, UNOPTIMIZED);
    SET_FLAG("-fO1", options_p->which_implm, OPTIMIZED);

    SET_FLAG_WITH_STRING("-fmatrix=", options_p->matrix_dims);
    SET_FLAG_WITH_STRING("-fm=", options_p->matrix_dims);

    SET_FLAG("-fupper", options_p->is_upper_triangular, true);
    SET_FLAG("-fu", options_p->is_upper_triangular, true);
    
    END_FLAG_READER();
}

EXERCISE_IMPLM_DECL(better_mul_unopt)
{
    uint32_t seed = 10000;
    double* A = calloc(
            options_p->columns*(options_p->is_upper_triangular ? options_p->columns : options_p->rows),
            sizeof(double));
    for (uint32_t i = 0; i < options_p->columns; i++)
    {
        for (uint32_t j = 0; j < (options_p->is_upper_triangular ? options_p->columns : options_p->rows); j++)
        {
            if (options_p->is_upper_triangular && j < i)
            {
                A[i + options_p->columns*j] = 0.0;
            } else {
                A[i + options_p->columns*j] = (double)rand_r(&seed)/(double)RAND_MAX;
            }
            seed++;
        }
    }

    double* x = calloc(
            options_p->rows,
            sizeof(double));
    for (uint32_t i = 0; i < options_p->rows; i++)
    {
        x[i] = (double)rand_r(&seed)/(double)RAND_MAX;
    }

    double* y = calloc(
            options_p->columns,
            sizeof(double));
    
    for (uint32_t i = 0; i < options_p->tries; i++)
    {
        uint32_t i, j, temp;
        BENCHMARK_T benchmark = start_benchmark();
        #pragma omp parallel for num_threads(options_p->job_count)  \
                default(none) private(i, j, temp)  shared(A, x, y, options_p)
        for (i = 0; i < options_p->columns; i++) 
        {
                temp = 0.0;
                for (j = 0; j < options_p->rows; j++) 
                {
                    temp += A[i + options_p->columns*j]*x[j];
                }
                y[i] = temp;
        }
        *time_of_execution_p += stop_benchmark(benchmark);
    }

    free(A);
    free(x);
    free(y);
}

EXERCISE_IMPLM_DECL(better_mul_opt)
{
    uint32_t seed = 10000;
    double* A = calloc(
            options_p->columns*(options_p->is_upper_triangular ? options_p->columns : options_p->rows),
            sizeof(double));
    for (uint32_t i = 0; i < options_p->columns; i++)
    {
        for (uint32_t j = 0; j < (options_p->is_upper_triangular ? options_p->columns : options_p->rows); j++)
        {
            if (options_p->is_upper_triangular && j < i)
            {
                A[i + options_p->columns*j] = 0.0;
            } else {
                A[i + options_p->columns*j] = (double)rand_r(&seed)/(double)RAND_MAX;
            }
            seed++;
        }
    }

    seed = 10000;
    double* x = calloc(
            options_p->rows,
            sizeof(double));
    for (uint32_t i = 0; i < options_p->rows; i++)
    {
        x[i] = (double)rand_r(&seed)/(double)RAND_MAX;
        seed++;
    }

    double* y = calloc(
            options_p->columns,
            sizeof(double));
    
    for (uint32_t i = 0; i < options_p->tries; i++)
    {
        uint32_t i, j, temp;
        BENCHMARK_T benchmark = start_benchmark();
        #pragma omp parallel for num_threads(options_p->job_count)  \
                default(none) private(i, j, temp)  shared(A, x, y, options_p)
        for (i = 0; i < options_p->columns; i++) 
        {
                temp = 0.0;
                for (j = i; j < options_p->columns - i; j++) 
                {
                    temp += A[i + options_p->columns*j]*x[j];
                }
                y[i] = temp;
        }
        RECORD(benchmark);
    }

    free(A);
    free(x);
    free(y);
}

