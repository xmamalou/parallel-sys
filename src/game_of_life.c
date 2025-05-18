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
#define __USE_XOPEN_EXTENDED
#undef __USE_XOPEN2K8

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
    SERIAL   = 0,
    PARALLEL = 1,
} Which;

typedef struct Options {
    Which    which_algo;
    uint32_t job_count;
    uint64_t generations;
    char     matrix_dims[4]; // 2 digits for each dimension, an `x` and null terminator
    char     data_path[PATH_MAX];
    uint32_t columns;
    uint32_t rows;
    bool*    A[2];
    uint32_t tries;
    bool     animate; // Whether to animate the game of life or not (show it in the terminal)
} Options;

// --- CONSTANTS --- //

#define BIG_BUFF 256

static const char* implm_string[] = {
    "serial",
    "parallel",
};

static const uint64_t nsec_to_sec_factor  = 1000000000;
static const uint64_t nsec_to_msec_factor = 1000000;

// --- GLOBALS --- //

/// @brief total amount of successful throws (i.e. inside the circle)
static uint64_t        succ_throws_g = 0;
static pthread_mutex_t throws_mtx;

// --- FUNCTION DECLARATIONS --- //

FLAG_READER_DECL();

/// @brief Checks if the cell at (i, j) is alive
/// @param A the matrix to check
/// @param columns the number of columns in the matrix
/// @param rows the number of rows in the matrix
/// @param i the row of the cell to check
/// @param j the column of the cell to check
/// @return whether the cell is alive or not
static bool is_alive(
        const bool* A, uint32_t columns, uint32_t rows,
        uint32_t i, uint32_t j);

/// @brief The original unoptimized version of the algorithm
EXERCISE_IMPLM_DECL(game_of_life_serial);
/// @brief The optimized version of the algorithm for upper triangular matrices
EXERCISE_IMPLM_DECL(game_of_life_parallel);

/// @brief Shows the matrix in the terminal
static void show_matrix(
        const bool* A, uint32_t columns, uint32_t rows,
        uint32_t generation);

// --- FUNCTION DEFINITIONS --- //

void game_of_life(
        char** flags, uint32_t flag_count) 
{
    Options options = {
        .job_count           = 1,
        .which_algo          = SERIAL,
        .generations         = 1,
        .matrix_dims         = "",
        .data_path           = "",
        .columns             = 0,
        .rows                = 0,
        .A                   = {NULL, NULL},
        .tries               = 1,
        .animate             = false,
    };
    read_flags(
            flags, flag_count,
            &options);

    // If the user wants to use stdout to output data, we suppress any messages to
    // allow only the output that would normally be logged to show up
    if ( strcmp(options.data_path, "") != 0 ) 
    {
        printf("\x1b[33mParallel Systems Postgrad Course -- Project 1 -- Christoforos-Marios Mamaloukas\n\n"
                "--- EXERCISE 6 (Game of Life) ---\n"
                "* You will run the %s version\n"
                "* You want %u jobs\n"
                "* The game will run for %d generations\n"
                "* The matrix will have dimensions %s\n"
                "* Data will be saved to %s\n"
                "* The experiment will be run %d tries\n"
                "* Animation is %s\n"
                "---------------------------------\n\n\x1b[0m",
                implm_string[options.which_algo],
                options.job_count, 
                options.generations,
                options.matrix_dims,
                options.data_path,
                options.tries,
                options.animate ? "enabled" : "disabled");
    }

    sscanf(
            options.matrix_dims,
            "%ux%u",
            &options.columns,
            &options.rows);

    uint32_t seed = 10000;
    // The algorithm will use one matrix to update the other matrix
    // in a circular fashion
    options.A[0] = (bool*)calloc(
            options.columns*options.rows,
            sizeof(bool));
    options.A[1] = (bool*)calloc(
            options.columns*options.rows,
            sizeof(bool));
            
    // Fill the first matrix with random values
    for (uint32_t i = 0; i < options.columns; i++)
    {
        for (uint32_t j = 0; j < options.rows; j++)
        {
                options.A[0][i + options.columns*j] = rand_r(&seed) % 2 == 0;
                options.A[1][i + options.columns*j] = rand_r(&seed) % 2 == 0;
        }
    }

    double time_of_execution = 0;
    EXERCISE_IMPLM_T functions[] = {
        &game_of_life_serial,
        &game_of_life_parallel,
    };
    functions[options.which_algo](
            &options,
            &time_of_execution);

    CALCULATE_TIME(time_of_execution);

    LOG(
            "[EXERCISE 6]\ntype = %s\njobs = %d\nmatrix = %s\ngenerations = %d\ntime = %f\n",
            implm_string[options.which_algo],
            options.job_count,
            options.matrix_dims,
            options.generations,
            time_of_execution);

    free(options.A[0]);
    free(options.A[1]);

    return;
}

FLAG_READER(options_p)
{
    SET_FLAG("-fs", options_p->which_algo, SERIAL);
    SET_FLAG("-fserial", options_p->which_algo, SERIAL);

    SET_FLAG("-fp", options_p->which_algo, PARALLEL);
    SET_FLAG("-fparallel", options_p->which_algo, PARALLEL);

    SET_FLAG_WITH_NUM("-fgen", options_p->generations, ll);
    SET_FLAG_WITH_NUM("-fg=", options_p->generations, ll);

    SET_FLAG_WITH_STRING("-fmatrix=", options_p->matrix_dims);
    SET_FLAG_WITH_STRING("-fm=", options_p->matrix_dims);

    SET_FLAG("-fanimate", options_p->animate, true);
    SET_FLAG("-fa", options_p->animate, true);

    END_FLAG_READER();
}

static bool is_alive(
        const bool* A, 
        uint32_t columns, uint32_t rows,
        uint32_t i, uint32_t j)
{
    uint32_t top_i = i == 0 
            ? i 
            : i - 1;
    uint32_t top_j = j == 0 
            ? j 
            : j - 1;
    
    uint32_t bottom_i = i == rows - 1
            ? i 
            : i + 1;
    uint32_t bottom_j = j == columns - 1
            ? j 
            : j + 1;

    uint32_t alive_neighbors = 0;
    for (uint32_t x = top_i; x <= bottom_i; x++)
    {
        for (uint32_t y = top_j; y <= bottom_j; y++)
        {
            // Skip the cell itself
            if (x == i && y == j)
            {
                continue;
            }

            if (A[x + columns*y] == true)
            {
                alive_neighbors++;
            }
        }
    }

    if (alive_neighbors == 3 && A[i + j*columns] == false)
    {
        return true;
    } else if (alive_neighbors > 1 && alive_neighbors < 4 && A[i + j*columns] == true)
    {
        return true;
    } else {
        return false;
    }
}

EXERCISE_IMPLM_DECL(game_of_life_serial)
{
    uint8_t  which_matrix = 1;
    
    for (uint32_t l = 0; l < options_p->tries; l++)
    {
        BENCHMARK_T benchmark = start_benchmark();
        for (uint32_t k = 0; k < options_p->generations; k++)
        {
            for (uint32_t i = 0; i < options_p->columns; i++)
            {
                for (uint32_t j = 0; j < options_p->rows; j++)
                {
                    options_p->A[1 - which_matrix % 2][i + j*options_p->columns] = is_alive(
                            options_p->A[which_matrix % 2], 
                            options_p->columns, options_p->rows,
                            i, j);
                }
            }
            which_matrix++;

            if (options_p->animate)
            {
                system("clear");
                show_matrix(
                        options_p->A[1 - which_matrix % 2], 
                        options_p->columns, options_p->rows,
                        k);
                usleep(160000);
            }
        }
        RECORD(benchmark);
    }
}

EXERCISE_IMPLM_DECL(game_of_life_parallel)
{
    uint8_t  which_matrix = 1;
    
    for (uint32_t l = 0; l < options_p->tries; l++)
    {
        BENCHMARK_T benchmark = start_benchmark();
        bool* A[2] = {
            options_p->A[0],
            options_p->A[1],
        };
        uint32_t columns = options_p->columns, 
                rows = options_p->rows, 
                generations = options_p->generations, 
                i, j;
        bool animate = options_p->animate;
        #pragma omp parallel for num_threads(options_p->job_count) \
                default(none) private(i, j) shared( \
                        A, columns, rows, generations, which_matrix, animate)
        for (uint32_t k = 0; k < generations; k++)
        {
            for (uint32_t i = 0; i < columns; i++)
            {
                for (uint32_t j = 0; j < rows; j++)
                {
                    A[1 - which_matrix % 2][i + j*columns] = is_alive(
                            A[which_matrix % 2], 
                            columns, rows,
                            i, j);
                }
            }
            which_matrix++;
        }
        RECORD(benchmark);
    }
}

static void show_matrix(
        const bool* A, uint32_t columns, uint32_t rows,
        uint32_t generation)
{
    printf("Generation %d\n", generation);
    for (uint32_t i = 0; i < columns; i++)
    {
        for (uint32_t j = 0; j < rows; j++)
        {
            if (A[i + j*columns] == true)
            {
                printf("█");
            } else {
                printf("┼");
            }
        }
        printf("\n");
    }
}
