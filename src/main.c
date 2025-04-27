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

#include <omp.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "exercises.h"

// --- TYPES --- //

/// @brief Exercise number
typedef enum Exercise {
    HELP       = 0,
    EXERCISE_1 = 1,
    EXERCISE_2 = 2,
    EXERCISE_3 = 3,
    EXERCISE_4 = 4,
    EXERCISE_5 = 5,
    EXERCISE_6 = 6,
    EXERCISE_7 = 7,
    EXERCISE_8 = 8,
    EXERCISE_9 = 9,
    LIST       = 10,
} Exercise;

/// @brief Options for the project
typedef struct Options {
    Exercise which;
    char**   flags;
    uint32_t flag_count;
} Options;

typedef void (*ExerciseFunc)(
        char** flags, uint32_t flag_count);

// --- CONSTANTS --- //

const ExerciseFunc exercise_funcptrs[] = {
    pi_calc
};

// --- FUNCTIONS --- //

/// @brief Reads arguments from the command line to parse them into actions
/// @param argc command line arguments
/// @param argv command line argument count
/// @param exercise_options structure to be modified depending on command line arguments
void read_arguments(
        int argc, char* argv[],
        Options* exercise_options);

/// @brief Parses a string of `-f` flags to pass them to an exercise. Flags must be sequential and not be broken
/// by another non-flag argument.  
/// @param argv_slice 
/// @param argv_slice_length 
/// @return The amount of arguments that refer to flags
uint32_t count_flags(
        char** argv_slice, uint32_t argv_slice_length);

// --- FUNC DEFINITIONS --- //

int main(
        int argc, char* argv[]) 
{
    Options exercise_options = {
        .which      = HELP,
        .flags      = NULL,
        .flag_count = 0
    };

    if(argc > 1) 
    {
        read_arguments(
                argc, argv,
                &exercise_options);
    }

    // Help just prints a message and quits; program does not proceed.
    if (exercise_options.which == HELP) 
    {
        printf("\x1b[33mParallel Systems Postgrad Course -- Project 1 -- Christoforos-Marios Mamaloukas\n"
                "Available options:\n"
                "\t* -h, --help: Shows the current message\n"
                "\t* -l, --list: List exercises and supported flags\n"
                "\t* -e <number>, --exercise <number>: Executes Exercise <number>. Must be at least 1 and at most 9\n\x1b[0m");
        return 0;
    } else if (exercise_options.which == LIST) {
        printf("\x1b[33mParallel Systems Postgrad Course -- Project 1 -- Christoforos-Marios Mamaloukas\n"
                "Exercises:\n"
                "\t(1) Calculate π using the Monte Carlo method\n"
                "\tFlags (prefixed with -f):\n"
                "\t\t* s, serial: Run the serial version of the algorithm\n"
                "\t\t* p, parallel: Run the parallel version of the algorithm (default)\n"
                "\t\t* omp: Run the openmp version of the parallel algorithm (if not specified, runs the pthread version)\n"
                "\t\t* j=<number>, jobs=<number>: Number of threads for the parallel version of an algorithm\n"
                "\t\t* n=<number>, throws=<number>: Number of throws for the Monte Carlo experiment\n"
                "\t\t* f=<path>, file=<path>: Path to the file to save the data in; leave empty to use stdout\n"
                "\t\t* t=<number>, tries=<number>: Number of tries for the experiment. Logged data is the average execution time and value of π\x1b[0m");
        return 0;
    }

    exercise_funcptrs[exercise_options.which](
            exercise_options.flags, exercise_options.flag_count);

    return 0;
}

//

void read_arguments(
        int argc, char* argv[],
        Options* exercise_options) 
{
    exercise_options->which = HELP;

    for (uint32_t i = 1; i < argc; i++)
    {
        // Help causes the program to ignore all other arguments
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) 
        {
            exercise_options->which = HELP;
            return;
        }

        if (strcmp(argv[i], "--exercise") == 0 || strcmp(argv[i], "-e") == 0) 
        {
            if (i + 1 < argc 
                    && (atoi(argv[i+1]) < 10 && atoi(argv[i+1]) > 0)) 
            {
                exercise_options->which = (Exercise)atoi(argv[i+1]); 
                exercise_options->flag_count = count_flags(
                        &(argv[i+2]), argc - i - 2);
                exercise_options->flags = &(argv[i+2]);

                i += exercise_options->flag_count + 1; // besides the flag counts, there's also the exercise number
                continue;
            } else {
                fprintf(stderr, "\x1b[31mERROR! Expected exercise number but was given no or invalid parameter!!\x1b[0m\n");
                return; // Invalid parameter, we cannot continue
            }
        }

        if (strcmp(argv[i], "--list") == 0 || strcmp(argv[i], "-l") == 0)
        {
            exercise_options->which = LIST;
            return;
        }
        
        // If any of the above checks fails, this means an invalid parameter was passed.
        // Quit and show help message 
        fprintf(stderr, "\x1b[31mERROR! Argument %s is unknown!\n", argv[i]);
        exercise_options->which = HELP;
        break; 
    }
}

uint32_t count_flags(
        char** argv_slice, uint32_t argv_slice_length)
{
    uint32_t count = 0;
    for (uint32_t i = 0; i < argv_slice_length; i++)
    {
        if (strstr(argv_slice[i], "-f") != NULL)
        {
            count++;
        } else {
             // if the array of flags is broken by something that does not start with `-f`, we consider that there are no
             // more flags for the exercise
            break;
        }
    }

    return count;
}
