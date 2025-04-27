#!/usr/bin/env -S nu --stdin

def main [
    --from      : int    = 10000000 # Initial amount of throws to try
    --to        : int    = 100000000 # Final amount of throws to try
    --step      : int    = 20000000 # How much to add to the next amount of throws to try
    --times     : int    = 10 # How many times to repeat the experiment for each throw
    --jobs-from : int    = 2 # How many jobs to start initially
    --jobs-to   : int    = 32 # How many jobs to start in the last attempts
    --jobs-step : int    = 4 # Step for job count 
] {
    if ( "data/out1.txt" | path exists ) { rm data/out1.txt }

    let second = $from + $step
    let jobs_second = $jobs_from + $jobs_step

    # first, we do serial
    for $it in $from..$second..$to {
        ./build/exe -e 1 -fs $"-fn=($it)" -ffile=data/out1.txt $"-ft=($times)" | ignore
    }

    # next, parallel with pthreads
    for $it in $from..$second..$to {
        for $jt in $jobs_from..$jobs_second..$jobs_to {
            ./build/exe -e 1 -fp $"-fj=($jt)" $"-fn=($it)" -ffile=data/out1.txt $"-ft=($times)" | ignore
        }
    }

    # we finish off with openmp
    for $it in $from..$second..$to {
        for $jt in $jobs_from..$jobs_second..$jobs_to {
            ./build/exe -e 1 -fomp $"-fj=($jt)" $"-fn=($it)" -ffile=data/out1.txt $"-ft=($times)" | ignore
        }
    }
}