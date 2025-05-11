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
    uint32_t tries;
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

static void read_flags(
        char** flags, uint32_t flag_count,
        Options* options_p);

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
static void game_of_life_serial(const Options* options_p);
/// @brief The optimized version of the algorithm for upper triangular matrices
static void game_of_life_parallel(const Options* options_p);

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
                "--- EXERCISE 5 (Game of Life) ---\n"
                "* You will run the %s version\n"
                "* You want %u jobs\n"
                "* The game will run for %s generations\n"
                "* The matrix will have dimensions %s\n"
                "* Data will be saved to %s\n"
                "* The experiment will be run %d tries\n"
                "---------------------------------\n\n\x1b[0m",
                implm_string[options.which_algo],
                options.job_count, 
                options.generations,
                options.matrix_dims,
                options.data_path,
                options.tries);
    }

    typedef void (*Implementations)(const Options*);
    Implementations functions[] = {
        &game_of_life_serial,
        &game_of_life_parallel,
    };
    functions[options.which_algo](&options);

    return;
}

static void read_flags(
    char** flags, uint32_t flag_count,
    Options* options_p)
{
    for (uint32_t i = 0; i < flag_count; i++)
    {
        if (strstr(flags[i], "-fs") != NULL || strstr(flags[i], "-fserial") != NULL)
        {
            fprintf(stderr,
                    "\x1b[31mHey! You requested the serial version, even though"
                    " you already want the parallel one! IGNORING!\n\x1b[0m");
            options_p->which_algo = SERIAL;
        }

        if (strstr(flags[i], "-fp") != NULL || strstr(flags[i], "-fparallel") != NULL)
        {

            options_p->which_algo = PARALLEL;
        }

        if (strstr(flags[i], "-fjobs=") != NULL || strstr(flags[i], "-fj=") != NULL)
        {
            char* equal_char_p = strchr(flags[i], '=');
            options_p->job_count = atoi(&(equal_char_p[1])); // same as (equal_char_p + sizeof(char)), allows us to get the number next to the `=` sign
        }

        if (strstr(flags[i], "-fgenerations=") != NULL || strstr(flags[i], "-fg=") != NULL)
        {
            char* equal_char_p = strchr(flags[i], '=');
            options_p->generations = atoi(&(equal_char_p[1])); // same as (equal_char_p + sizeof(char)), allows us to get the number next to the `=` sign
        }

        if (strstr(flags[i], "-fmatrix=") != NULL || strstr(flags[i], "-fm=") != NULL)
        {
            char* equal_char_p     = strchr(flags[i], '=');
            strncat(
                    options_p->matrix_dims,
                    &equal_char_p[1],
                    strlen(&equal_char_p[1]));
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
            if (A[x*columns + y] == true)
            {
                alive_neighbors++;
            }
        }
    }
}

static void game_of_life_serial(const Options* options_p) 
{
    LOG_T log = NULL;
    if ( strcmp(options_p->data_path, "") != 0 )
    {
        printf("Working on that array... ");

        log = open_log(
                options_p->data_path,
                true); 
    }

    uint32_t columns = 0, rows = 0;
    sscanf(
            options_p->matrix_dims,
            "%ux%u",
            &columns,
            &rows);

    uint32_t seed = 10000;
    // The algorithm will use one matrix to update the other matrix
    // in a circular fashion
    bool* A[2] = {
            (bool*)calloc(
                    columns*rows,
                    sizeof(bool)),
            (bool*)calloc(
                    columns*rows,
                    sizeof(bool)),
    };

    uint64_t avg_time = 0;
    uint8_t  which_matrix = 0;
    
    for (uint32_t i = 0; i < options_p->tries; i++)
    {
        BENCHMARK_T benchmark = start_benchmark();
        for (uint32_t k = 0; k < options_p->generations; k++)
        {
            for (uint32_t i = 0; i < columns; i++)
            {
                for (uint32_t j = 0; j < rows; j++)
                {
                    A[which_matrix][i*columns + j] = is_alive(
                            A[1 - which_matrix], 
                            columns, rows,
                            i, j);
                }
            }
            which_matrix = ++which_matrix % 2;
        }
        avg_time += stop_benchmark(benchmark);
    }

   if ( strcmp(options_p->data_path, "") != 0 )
    {
        
        printf("This took %f msecs!", 
                (double)avg_time/(double)nsec_to_msec_factor);

        char text[PATH_MAX] = "";
        sprintf(
                text,
                "[EXERCISE 6]\ntype = %s\njobs = %d\nmatrix = %s\ngenerations = %d\ntime = %f\n",
                implm_string[options_p->which_algo],
                options_p->job_count,
                options_p->matrix_dims,
                options_p->generations,
                (double)avg_time/(double)nsec_to_msec_factor);
        write_log(
                log,
                text);
        close_log(log);
    } else {
        printf(
                "[EXERCISE 6]\ntype = %s\njobs = %d\nmatrix = %s\ngenerations = %d\ntime = %f\n",
                implm_string[options_p->which_algo],
                options_p->job_count,
                options_p->matrix_dims,
                options_p->generations,
                (double)avg_time/(double)nsec_to_msec_factor);
    } 

    free(A[1]);
    free(A[0]);
}

static void game_of_life_parallel(const Options* options_p) 
{
    
}
