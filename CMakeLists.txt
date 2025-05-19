cmake_minimum_required(VERSION 3.10)

project(parallel_systems_vol1
        VERSION 0.0.1
        DESCRIPTION "First set of exercises from the 'Parallel Systems' course of the 'Computer Engineering' masters programme of the Informatics and Telecommunications Department of the University of Athens"
        LANGUAGES C)

set(APP_NAME exe)

include_directories(
        src/)

set(SRCFILES 
        src/main.c 
        src/pi_calc.c
        src/shared_var.c
        src/false_sharing.c
        src/barriers.c
        src/better_mul.c
        src/game_of_life.c
        src/gaussian_elimination.c
        src/merge_sort.c
        src/utility.c)
set(THREADS_PREFER_PTHREAD_FLAG ON)

if("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
        set(APP_NAME exe-dbg)      
endif()

add_executable(${APP_NAME} ${SRCFILES})
target_compile_options(${APP_NAME} PRIVATE -D_POSIX_C_SOURCE=200809L)
find_package(Threads REQUIRED)
find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()
target_link_libraries(${APP_NAME} PRIVATE Threads::Threads)