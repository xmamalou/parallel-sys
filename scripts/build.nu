#!/usr/bin/env -S nu --stdin

def main [
        --jobs: int = 1 # How many processors to use for building
        --debug # Create a debug build
        --no-weeb # Do not use ninja for building
] {
    cd $"($env.PWD)/build"

    # This allows using a different build method at a later time
    if ("./CMakeCache.txt" | path exists) { 
        print $"(ansi red)Found cache from previous build! Deleting...\n(ansi reset)"
        rm ./CMakeCache.txt
    }

    let build_type = if $debug {"Debug"} else {"Release"}

    if not $no_weeb and ("/usr/bin/ninja" | path exists) {
        print $"(ansi red)Using (ansi purple)Ninja(ansi red) as build method\n(ansi green)"
        cmake $"-DCMAKE_BUILD_TYPE=($build_type)" -G Ninja ..
        ninja -j $jobs    
    } else {
        print $"(ansi red)Using (ansi purple)Make(ansi red) as build method\n(ansi green)"
        cmake $"-DCMAKE_BUILD_TYPE=($build_type)" -G "Unix Makefiles" .. 
        make -j $jobs
    }
    
    print $"(ansi reset)"
}
