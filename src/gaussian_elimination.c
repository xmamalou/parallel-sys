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
#include <unistd.h>
#include <linux/limits.h>
#include <omp.h>

#include "utility.h"
#include "macros.h"

// -- TYPES --- //

typedef enum Which {
    SERIAL   = 0,
    PARALLEL = 1,
} Which;

typedef enum Algo {
    PER_COLUMN = 0,
    PER_ROW    = 1,
} Algo;

typedef struct Options {
    uint32_t job_count;
    Which    which_implm;
    Algo     algo_type;
    uint64_t matrix_dims;
    double*  A;
    double*  x;
    double*  b;
    char     data_path[PATH_MAX];
    uint32_t tries;
} Options;

// --- CONSTANTS --- //

#define BIG_BUFF 256

static const char* implm_string[] = {
    "serial",
    "parallel",
};

static const char* algo_type[] = {
    "per column",
    "per row",
};

static const uint64_t nsec_to_sec_factor  = 1000000000;
static const uint64_t nsec_to_msec_factor = 1000000;

// --- FUNCTION DECLARATIONS --- //

FLAG_READER_DECL();

/// @brief Serial + per-column implementation
EXERCISE_IMPLM_DECL(gaussian_serial_pc);
/// @brief Serial + per-row implementation
EXERCISE_IMPLM_DECL(gaussian_serial_pr);
/// @brief Parallel + per-column implementation
EXERCISE_IMPLM_DECL(gaussian_parallel_pc);
/// @brief Parallel + per-row implementation
EXERCISE_IMPLM_DECL(gaussian_parallel_pr);

// --- FUNCTION DEFINITIONS --- //

// This exercise was by far the most painful, and for all the wrong/irrelevant reasons.
// From here on, I fully support a Rust-ful and C-less future.
void gaussian_elimination(
        char** flags, uint32_t flag_count) 
{
    Options options = {
        .job_count           = 1,
        .which_implm         = SERIAL,
        .algo_type           = PER_ROW,
        .matrix_dims         = 0,
        .A                   = NULL,
        .x                   = NULL,
        .b                   = NULL,
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
                "--- EXERCISE 7 (Gaussian elimination) ---\n"
                "* You will run the %s version\n"
                "* You will run the %s algorithm\n"
                "* You want %u jobs\n"
                "* The matrix will have dimensions %dx%d\n"
                "* Data will be saved to %s\n"
                "* The experiment will be run %d tries\n"
                "-------------------------------------------------\n\n\x1b[0m",
                implm_string[options.which_implm],
                algo_type[options.algo_type],
                options.job_count, 
                options.matrix_dims,
                options.matrix_dims,
                options.data_path,
                options.tries);
    }


    // There was supposed to be a much more elegant way to create matrices, but
    // because malloc is sadistic, I decided to do it the ugly way

    FILE* urandom = fopen("/dev/urandom", "r");
    options.A = calloc(
            options.matrix_dims*options.matrix_dims,
            sizeof(double));
    for (uint32_t i = 0; i < options.matrix_dims; i++)
    {
        for (uint32_t j = 0; j < options.matrix_dims; j++)
        {
            if (j < i)
            {
                options.A[i + options.matrix_dims*j] = 0.0; // I wish C had lvalue references
            } else {
                uint8_t rand_num;
                fread(&rand_num, sizeof(uint8_t), 1, urandom);
                options.A[i + options.matrix_dims*j] = (uint32_t)rand_num;
            }
        }
    }

    options.b = calloc(
            options.matrix_dims,
            sizeof(double));
    for (uint32_t i = 0; i < options.matrix_dims; i++)
    {
        uint32_t rand_num;
        fread(&rand_num, sizeof(uint32_t), 1, urandom);
        options.b[i] = (double)rand_num/(double)RAND_MAX;
    }
    fclose(urandom);

    options.x = calloc(
            options.matrix_dims,
            sizeof(double));
    memcpy(
            options.x,
            options.b,
            options.matrix_dims*sizeof(double));

    double time_of_execution = 0;

    EXERCISE_IMPLM_T functions[] = {
        &gaussian_serial_pc,
        &gaussian_serial_pr,
        &gaussian_parallel_pc,
        &gaussian_parallel_pr
    };
    functions[(options.which_implm << 1) + options.algo_type](
            &options,
            &time_of_execution);

    CALCULATE_TIME(time_of_execution);

    LOG(
            "[EXERCISE 7]\ntype = %s\nalgo = %s\ndims = %dx%d\njobs = %u\ntime = %f\n",
            implm_string[options.which_implm],
            algo_type[options.algo_type],
            options.matrix_dims,
            options.matrix_dims,
            options.job_count,
            time_of_execution);

    free(options.A);
    free(options.x);

    return;
}

FLAG_READER(options_p)
{
    // Which implementation to use
    SET_FLAG("-fs", options_p->which_implm, SERIAL);
    SET_FLAG("-fserial", options_p->which_implm, SERIAL);
    SET_FLAG("-fp", options_p->which_implm, PARALLEL);
    SET_FLAG("-fparallel", options_p->which_implm, PARALLEL);

    // Which algorithm to use
    SET_FLAG("-fpc", options_p->algo_type, PER_COLUMN);
    SET_FLAG("-fper_column", options_p->algo_type, PER_COLUMN);
    SET_FLAG("-fpr", options_p->algo_type, PER_ROW);
    SET_FLAG("-fper_row", options_p->algo_type, PER_ROW);

    SET_FLAG_WITH_NUM("-fmatrix=", options_p->matrix_dims, ll);
    SET_FLAG_WITH_NUM("-fm=", options_p->matrix_dims, ll);
    
    END_FLAG_READER();
}

EXERCISE_IMPLM_DECL(gaussian_serial_pc)
{
    for (uint64_t k = 0; k < options_p->tries; k++)
    {
        BENCHMARK_T benchmark = start_benchmark();
        for (uint64_t i = 0; i < options_p->matrix_dims; i++)
        {
            options_p->x[i] = options_p->b[i];
        }

        for (int64_t i = options_p->matrix_dims - 1; i >= 0; i--)
        {
            options_p->x[i] /= options_p->A[i + options_p->matrix_dims*i];
            for (int64_t j = 0; j < i; j++)
            {
                options_p->x[j] -= options_p->A[j + options_p->matrix_dims*i]*options_p->x[i];
            }
        }
        RECORD(benchmark);
    }
}

EXERCISE_IMPLM_DECL(gaussian_serial_pr)
{
    for (uint64_t k = 0; k < options_p->tries; k++)
    {
        BENCHMARK_T benchmark = start_benchmark();
        for (
                int64_t i = options_p->matrix_dims - 1; 
                i >= 0; 
                i--) 
        {
            options_p->x[i] = options_p->b[i];
            for (int64_t j = i+1; j < options_p->matrix_dims; j++) 
            {
                options_p->x[i] -= options_p->A[i + options_p->matrix_dims*j]*options_p->x[j];
            }
            options_p->x[i] /= options_p->A[i + options_p->matrix_dims*i];
        }
        RECORD(benchmark);
    }
}

EXERCISE_IMPLM_DECL(gaussian_parallel_pc)
{
    for (uint64_t k = 0; k < options_p->tries; k++)
    {
        BENCHMARK_T benchmark = start_benchmark();
        {
            #pragma omp parallel for num_threads(options_p->job_count)
            for (uint64_t i = 0; i < options_p->matrix_dims; i++)
            {
                options_p->x[i] = options_p->b[i];
            }

            for (uint32_t i = 0; i < options_p->matrix_dims; i++)
            {
                options_p->x[FLIP_INDEX(i, options_p->matrix_dims)] /= options_p->A[i + options_p->matrix_dims*i];
                #pragma omp parallel for num_threads(options_p->job_count) 
                for (uint64_t j = i+1; j < options_p->matrix_dims; j++)
                {
                    options_p->x[FLIP_INDEX(i, options_p->matrix_dims)] 
                            -= options_p->A[i + options_p->matrix_dims*j]*options_p->x[FLIP_INDEX(j, options_p->matrix_dims)];
                }
            }
        }
        RECORD(benchmark);
    }
}

EXERCISE_IMPLM_DECL(gaussian_parallel_pr)
{
    for (uint64_t k = 0; k < options_p->tries; k++)
    {
        BENCHMARK_T benchmark = start_benchmark();
        for (int32_t i = options_p->matrix_dims - 1; i >= 0; i--)
        {
            options_p->x[i] = options_p->b[i];
            #pragma omp parallel for num_threads(options_p->job_count) \
                shared(i)
            for (int32_t j = i+1; j < options_p->matrix_dims; j++)
            {
                options_p->x[i] -= options_p->A[i + options_p->matrix_dims*j]*options_p->x[j];
            }
            options_p->x[i] /= options_p->A[i + options_p->matrix_dims*i];
        }
        RECORD(benchmark);
    }
}

