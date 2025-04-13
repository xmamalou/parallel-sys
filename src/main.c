#include <omp.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "exercises.h"

// --- TYPES --- //

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

typedef struct Options {
    Exercise which;
    bool     do_serial;
} Options;

// --- FUNCTIONS --- //

void read_arguments(
        int argc, char* argv[],
        Options *exercise_options);

// --- FUNC DEFINITIONS --- //

int main(
        int argc, char* argv[]) 
{
    Options exercise_options = {
        .which = HELP,
        .do_serial = false
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
                "\t\t* p, parallel: Run the parallel version of the algorithm (default)\n\x1b[0m");
        return 0;
    }

    switch(exercise_options.which) 
    {
    case EXERCISE_1:
        pi_calc(exercise_options.do_serial);
        break;
    default:
        printf("Hello World\n");
    }

    return 0;
}

//

void read_arguments(
        int argc, char* argv[],
        Options *exercise_options) 
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
                    && (atoi(argv[i+1]) < 10 && atoi(argv[i+1]) > 0)) {
                    exercise_options->which = (Exercise)atoi(argv[i+1]); 
                if (i + 2 < argc)
                {
                    if(strcmp(argv[i+2], "-fserial") == 0 || strcmp(argv[i+2], "-fs") == 0)
                    {
                        exercise_options->do_serial = true;
                    }

                    if(strcmp(argv[i+2], "-fparallel") == 0 || strcmp(argv[i+2], "-fp") == 0)
                    {
                        exercise_options->do_serial = false;
                    }

                    i += 2;
                } else {
                    i++;
                }
                continue;
            }  else {
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

