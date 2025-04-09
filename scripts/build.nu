#!/usr/bin/env -S nu --stdin

def main [
        --jobs: int = 1] {
    
    cd $"($env.PWD)/build"

    cmake -G Ninja ..
    ninja -j $jobs
}
