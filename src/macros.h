#ifndef _MACROS_H_
#define _MACROS_H_

#define EXERCISE_DECL(exercise_name) \
    void exercise_name(\
        char** flags, uint32_t flag_count)

#define EXERCISE_IMPLM_DECL(implementation_name) \
    static void implementation_name(const Options* options_p, double* time_of_execution_p)

#define EXERCISE_IMPLM_T typedef void (*Implementation)(const Options*, double*); Implementation 
#define CALLBACK_T typedef void* (*Callback)(void*); Callback

#define CALLBACK_DECL(implementation_name) \
    static void* implementation_name##_callback(\
        void* args)

#define FLAG_READER_DECL() \
    static void read_flags(\
        char** flags, uint32_t flag_count,\
        Options* options_p)

/// Define a function to read flags from the command line
#define FLAG_READER(options_p) \
    static void read_flags(\
        char** flags, uint32_t flag_count,\
        Options* options_p)\
    {\
        for (uint32_t i = 0; i < flag_count; i++)

#define SET_FLAG(string, option, value_if) \
    if (strcmp(flags[i], string) == 0) \
    { \
        option = value_if; \
        continue; \
    } 

#define SET_FLAG_WITH_NUM(string, option, type_if) \
    if (strstr(flags[i], string) != NULL) \
    { \
        char* equal_char_p = strchr(flags[i], '='); \
        option = ato##type_if(&(equal_char_p[1])); \
        continue; \
    }
#define SET_FLAG_WITH_STRING(string, option) \
    if (strstr(flags[i], string) != NULL) \
    { \
        char* equal_char_p = strchr(flags[i], '='); \
        strncat( \
                option, \
                &equal_char_p[1], \
                strlen(&equal_char_p[1])); \
        continue; \
    }

#define SET_FLAG_WITH_FILE(string, option) \
    if (strstr(flags[i], string) != NULL) \
    { \
        char* equal_char_p     = strchr(flags[i], '='); \
        if (equal_char_p[1] != '~' || equal_char_p[1] != '/') \
        { \
            getcwd(option, PATH_MAX); \
            strcat(option, "/"); \
        } \
        strncat( \
                option, \
                &equal_char_p[1], \
                strlen(&equal_char_p[1])); \
        continue; \
    }

#define END_FLAG_READER() \
    SET_FLAG_WITH_NUM("-fjobs=", options_p->job_count, i);\
    SET_FLAG_WITH_NUM("-fj=", options_p->job_count, i);\
    SET_FLAG_WITH_NUM("-ftries=", options_p->tries, i);\
    SET_FLAG_WITH_NUM("-ft=", options_p->tries, i);\
    SET_FLAG_WITH_FILE("-ffile=", options_p->data_path);\
    SET_FLAG_WITH_FILE("-ff=", options_p->data_path);\
    fprintf(stderr,\
            "\x1b[31mHey! You provided an invalid flag: %s\nIGNORING!\n\x1b[0m",\
            flags[i]);\
}\


/// Log a message to either the console or a file
#define LOG(string, ...) \
    if ( strcmp(options.data_path, "") != 0 ) \
    { \
        LOG_T log = open_log( \
            options.data_path, \
            true); \
        printf(string, __VA_ARGS__); \
        char text[PATH_MAX] = ""; \
        sprintf( \
                text, \
                string, __VA_ARGS__); \
        write_log( \
                log, \
                text); \
        close_log(log); \
    } else { \
        printf(string, __VA_ARGS__); \
    }

/// Record the current benchmark time
#define RECORD(benchmark) \
    *time_of_execution_p += stop_benchmark(benchmark)

/// Calculate the average time of execution
#define CALCULATE_TIME(time_of_exec) \
    time_of_exec /= options.tries; \
    time_of_exec /= nsec_to_msec_factor

/// Print a warning message
#define ERROR(string, ...) \
    fprintf(stderr, "\x1b[31m"); \
    fprintf(stderr, string, __VA_ARGS__); \
    fprintf(stderr, "\x1b[0m\n"); \

#endif /* _MACROS_H_ */