#!/usr/bin/env -S nu --stdin

def main [
        --jobs: int = 1 # How many processors to use for building
        --debug         # Create a debug build
        --no-weeb       # Do not use ninja for building
        --clang         # Use Clang instead of GCC as the compiler
] {
    cd $"($env.PWD)/build"

    # This allows using a different build method at a later time
    if ("./CMakeCache.txt" | path exists) { 
        print $"(ansi red)Found cache from previous build! Deleting...\n(ansi reset)"
        rm ./CMakeCache.txt
    }

    let build_type = if $debug {"Debug"} else {"Release"}
    let optimization = if $debug {"-DCMAKE_C_FLAGS_RELEASE=-O3"} else {""}
    let compiler_string = if $clang {"-DCMAKE_C_COMPILER=/bin/clang"} else {"-DCMAKE_C_COMPILER=/bin/gcc"}
    let build_gen = if not $no_weeb and ("/usr/bin/ninja" | path exists) {"Ninja"} else {"Unix Makefiles"}

    print $"(ansi red)Using (ansi purple)($build_gen)(ansi red) as build method\n(ansi green)"
    cmake $"-DCMAKE_BUILD_TYPE=($build_type)" $"($compiler_string)" $"($optimization)" -G $"($build_gen)" ..
    if not $no_weeb and ("/usr/bin/ninja" | path exists) {
        ninja -j $jobs    
    } else { .. 
        make -j $jobs
    }
    
    print $"(ansi reset)"
}
