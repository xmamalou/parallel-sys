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

#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <stdint.h>
#include <stdbool.h>

#define HANDLE_TYPE(name) name##Handle
#define HANDLE_BUILDER(name) struct HANDLE_TYPE(name)* HANDLE_TYPE(name)

// --- BENCHMARKING --- //

typedef HANDLE_BUILDER(Benchmark);
#define BENCHMARK_T HANDLE_TYPE(Benchmark)

/// @brief Start a benchmark 
/// @return Returns a handle to the used benchmark structure
inline BENCHMARK_T start_benchmark();

/// @brief Stop a benchmark. Frees the resources of the passed benchmark handle.
/// ALWAYS DISCARD THE HANDLE YOU PASSED IN THIS FUNCTION AFTERWARDS!
/// @param handle The benchmark to stop
/// @return the time that passed in nsec since `start_benchmark` was called
inline uint64_t stop_benchmark(BENCHMARK_T handle);

// --- LOGGING --- //

typedef HANDLE_BUILDER(Log);
#define LOG_T HANDLE_TYPE(Log)

/// @brief Open a log
/// @return Returns a handle to the created log
inline LOG_T open_log(
        const char* const path,
        const bool        do_append);

/// @brief Write to a log
/// @param log_h The log handle to write to
/// @param text The text to log
void write_log(
        const LOG_T handle, 
        const char* const text);

/// @brief Close a log
/// @param log_h The log handle to close
inline void close_log(const LOG_T handle);

// --- BARRIERS --- //

typedef HANDLE_BUILDER(Barrier);
#define BARRIER_T HANDLE_TYPE(Barrier)

/// @brief Create a barrier
/// @param holder_count how many will have to wait before the barrier can allow them to proceed
/// @return the handle to the barrier
inline BARRIER_T create_barrier(const uint32_t holder_count);

/// @brief Wait for a barrier
/// @param barrier_h the handle of the barrier to wait for
void waitfor_barrier(const BARRIER_T barrier_h);

/// @brief Destroy a barrier
/// @param barrier_h the handle of the barrier to destroy
inline void destroy_barrier(BARRIER_T barrier_h);

#endif /* __UTILITY_H__ */