#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <stdint.h>

typedef struct BenchmarkHandle* BenchmarkHandle;

/// @brief Start a benchmark 
/// @return Returns a handle to the used benchmark structure
inline BenchmarkHandle start_benchmark();

/// @brief Stop a benchmark. Frees the resources of the passed benchmark handle.
/// ALWAYS DISCARD THE HANDLE YOU PASSED IN THIS FUNCTION AFTERWARDS!
/// @param handle The benchmark to stop
/// @return the time that passed in nsec since `start_benchmark` was called
inline uint64_t        stop_benchmark(BenchmarkHandle handle);

#endif /* __UTILITY_H__ */