#!/usr/bin/env -S nu --stdin

def main [
    --mute                = false # Mute the output
    --jmp        : int    = 1 # The exercise to jump to
    --executable : string = "./build/exe" # Executable to run
    --tries      : int    = 4 # How many times to run each test
    --jobs       : int    = 4 # How many jobs to run in parallel
] {
    if ( "data/" | path exists ) { 
        for i in (1..9) {
            if ( $"data/exercise($i).out" | path exists ) {
                rm $"data/exercise($i).out"
            }
        }
    } else {
        mkdir data
    }

    # Check if the executable exists; if not, try to build it
    if not ($executable | path exists) {
        print $"(ansi red)Executable not found! Project will be built now\n(ansi reset)"
        if ( "scripts/build.nu" | path exists ) {
            exec "scripts/build.nu" -j 4 --no-weeb
        } else if ( "scripts/build.sh" | path exists )  {
            exec "scripts/build.sh" -j=4 --no-weeb
        } else {
            print $"(ansi red)Build script not found! Exiting...\n(ansi reset)"
            return 1
        }
    }

    # Run the exercises
    mut jmp = $jmp
    while $jmp <= 9 {
        match $jmp {
            1 => { 
                print $"(ansi purple)Running exercise 1\n(ansi purple)"
                # Exercise 1
                if not $mute {
                    nu -c $"exec ($executable) -e 1 -fs -fn=100000000 -ff=data/exercise1.out -ft=($tries) -fj=($jobs)" 
                    nu -c $"exec ($executable) -e 1 -fp -fn=100000000 -ff=data/exercise1.out -ft=($tries) -fj=($jobs)" 
                    nu -c $"exec ($executable) -e 1 -fomp -fn=100000000 -ff=data/exercise1.out -ft=($tries) -fj=($jobs)" 
                } else {
                    nu -c $"exec ($executable) -e 1 -fs -fn=100000000 -ff=data/exercise1.out -ft=($tries) -fj=($jobs)" | ignore
                    nu -c $"exec ($executable) -e 1 -fp -fn=100000000 -ff=data/exercise1.out -ft=($tries) -fj=($jobs)" | ignore
                    nu -c $"exec ($executable) -e 1 -fomp -fn=100000000 -ff=data/exercise1.out -ft=($tries) -fj=($jobs)" | ignore
                }
                # ##########
                print "\n"
                $jmp += 1 }
            2 => { 
                print $"(ansi purple)Running exercise 2\n(ansi purple)"
                # Exercise 2
                if not $mute {
                    nu -c $"exec ($executable) -e 2 -fl -fi=100000000 -ff=data/exercise2.out -ft=($tries) -fj=($jobs)" 
                    nu -c $"exec ($executable) -e 2 -fa -fi=100000000 -ff=data/exercise2.out -ft=($tries) -fj=($jobs)" 
                } else {
                    nu -c $"exec ($executable) -e 2 -fl -fi=100000000 -ff=data/exercise2.out -ft=($tries) -fj=($jobs)" | ignore
                    nu -c $"exec ($executable) -e 2 -fa -fi=100000000 -ff=data/exercise2.out -ft=($tries) -fj=($jobs)" | ignore
                }
                # ##########
                print "\n"
                $jmp += 1 }
            3 => { 
                print $"(ansi red)EXERCISE 3 IS CURRENTLY NOT WORKING! Skipping...\n(ansi red)"
                # TODO: Don't forget to remove the skip once this exercise is fixed
                $jmp += 1
                continue
                # Exercise 3
                if not $mute {
                    nu -c $"exec ($executable) -e 3 -fl -fi=100000000 -ff=data/exercise3.out -ft=($tries) -fj=($jobs)" 
                    nu -c $"exec ($executable) -e 3 -fa -fi=100000000 -ff=data/exercise3.out -ft=($tries) -fj=($jobs)" 
                    nu -c $"exec ($executable) -e 3 -fns -fi=100000000 -ff=data/exercise3.out -ft=($tries) -fj=($jobs)" 
                } else {
                    nu -c $"exec ($executable) -e 3 -fl -fi=100000000 -ff=data/exercise3.out -ft=($tries) -fj=($jobs)" | ignore
                    nu -c $"exec ($executable) -e 3 -fa -fi=100000000 -ff=data/exercise3.out -ft=($tries) -fj=($jobs)" | ignore
                    nu -c $"exec ($executable) -e 3 -fns -fi=100000000 -ff=data/exercise3.out -ft=($tries) -fj=($jobs)" | ignore
                }
                # ##########
                print "\n"
                $jmp += 1 }
            4 => { 
                print $"(ansi purple)Running exercise 4\n(ansi purple)"
                # Exercise 4
                # TODO: Don't forget to remove the skip once the busy wait barrier is fixed
                if not $mute {
                    nu -c $"exec ($executable) -e 4 -fp -fn=100000 -ff=data/exercise4.out -ft=($tries) -fj=($jobs)" 
                    nu -c $"exec ($executable) -e 4 -fc -fn=100000 -ff=data/exercise4.out -ft=($tries) -fj=($jobs)" 
                    #nu -c $"exec ($executable) -e 4 -fb -fn=100000000 -ff=data/exercise4.out -ft=($tries) -fj=($jobs)"
                } else {
                    nu -c $"exec ($executable) -e 4 -fp -fn=100000 -ff=data/exercise4.out -ft=($tries) -fj=($jobs)" | ignore
                    nu -c $"exec ($executable) -e 4 -fc -fn=100000 -ff=data/exercise4.out -ft=($tries) -fj=($jobs)" | ignore
                    #nu -c $"exec ($executable) -e 4 -fb -fn=100000000 -ff=data/exercise4.out -ft=($tries) -fj=($jobs)" | ignore
                }
                # ##########
                print "\n"
                $jmp += 1 }
            5 => { 
                print $"(ansi purple)Running exercise 5\n(ansi purple)"
                # Exercise 5
                if not $mute {
                    nu -c $"exec ($executable) -e 5 -fO0 -fm=10000x10000 -fu -ff=data/exercise5.out -ft=($tries) -fj=($jobs)" 
                    nu -c $"exec ($executable) -e 5 -fO1 -fm=10000x10000 -fu -ff=data/exercise5.out -ft=($tries) -fj=($jobs)" 
                } else {
                    nu -c $"exec ($executable) -e 5 -fO0 -fm=10000x10000 -fu -ff=data/exercise5.out -ft=($tries) -fj=($jobs)" | ignore
                    nu -c $"exec ($executable) -e 5 -fO1 -fm=10000x10000 -fu -ff=data/exercise5.out -ft=($tries) -fj=($jobs)" | ignore
                }
                # ##########
                print "\n"
                $jmp += 1 }
            6 => { 
                print $"(ansi purple)Running exercise 6\n(ansi purple)"
                # Exercise 6
                if not $mute {
                    nu -c $"exec ($executable) -e 6 -fs -fm=64x64 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)" 
                    nu -c $"exec ($executable) -e 6 -fs -fm=128x128 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)"
                    nu -c $"exec ($executable) -e 6 -fs -fm=256x256 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)"
                    nu -c $"exec ($executable) -e 6 -fs -fm=512x512 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)"
                    nu -c $"exec ($executable) -e 6 -fs -fm=1024x1024 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)"
                    nu -c $"exec ($executable) -e 6 -fs -fm=2048x2048 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)"
                    nu -c $"exec ($executable) -e 6 -fs -fm=4096x4096 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)"
                     nu -c $"exec ($executable) -e 6 -fp -fm=64x64 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)" 
                    nu -c $"exec ($executable) -e 6 -fp -fm=128x128 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)"
                    nu -c $"exec ($executable) -e 6 -fp -fm=256x256 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)"
                    nu -c $"exec ($executable) -e 6 -fp -fm=512x512 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)"
                    nu -c $"exec ($executable) -e 6 -fp -fm=1024x1024 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)"
                    nu -c $"exec ($executable) -e 6 -fp -fm=2048x2048 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)"
                    nu -c $"exec ($executable) -e 6 -fp -fm=4096x4096 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)" 
                } else {
                    nu -c $"exec ($executable) -e 6 -fs -fm=64x64 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)" | ignore
                    nu -c $"exec ($executable) -e 6 -fs -fm=128x128 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)" | ignore
                    nu -c $"exec ($executable) -e 6 -fs -fm=256x256 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)" | ignore
                    nu -c $"exec ($executable) -e 6 -fs -fm=512x512 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)" | ignore
                    nu -c $"exec ($executable) -e 6 -fs -fm=1024x1024 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)" | ignore
                    nu -c $"exec ($executable) -e 6 -fs -fm=2048x2048 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)" | ignore
                    nu -c $"exec ($executable) -e 6 -fs -fm=4096x4096 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)" | ignore
                     nu -c $"exec ($executable) -e 6 -fp -fm=64x64 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)"  | ignore
                    nu -c $"exec ($executable) -e 6 -fp -fm=128x128 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)" | ignore
                    nu -c $"exec ($executable) -e 6 -fp -fm=256x256 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)" | ignore
                    nu -c $"exec ($executable) -e 6 -fp -fm=512x512 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)" | ignore
                    nu -c $"exec ($executable) -e 6 -fp -fm=1024x1024 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)" | ignore
                    nu -c $"exec ($executable) -e 6 -fp -fm=2048x2048 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)" | ignore
                    nu -c $"exec ($executable) -e 6 -fp -fm=4096x4096 -fg=1000 -ff=data/exercise6.out -ft=($tries) -fj=($jobs)" | ignore
                }
                # ##########
                print "\n"
                $jmp += 1 }
            7 => { 
                print $"(ansi purple)Running exercise 7\n(ansi purple)"
                # Exercise 7
                if not $mute {
                    nu -c $"exec ($executable) -e 7 -fs -fpc -fm=10000x10000 -ff=data/exercise7.out -ft=($tries) -fj=($jobs)" 
                    nu -c $"exec ($executable) -e 7 -fs -fpr -fm=10000x10000 -ff=data/exercise7.out -ft=($tries) -fj=($jobs)" 
                    nu -c $"exec ($executable) -e 7 -fp -fpc -fm=10000x10000 -ff=data/exercise7.out -ft=($tries) -fj=($jobs)"
                    nu -c $"exec ($executable) -e 7 -fp -fpr -fm=10000x10000 -ff=data/exercise7.out -ft=($tries) -fj=($jobs)"
                } else {
                    nu -c $"exec ($executable) -e 7 -fs -fpc -fm=10000x10000 -ff=data/exercise7.out -ft=($tries) -fj=($jobs)" | ignore
                    nu -c $"exec ($executable) -e 7 -fs -fpr -fm=10000x10000 -ff=data/exercise7.out -ft=($tries) -fj=($jobs)" | ignore
                    nu -c $"exec ($executable) -e 7 -fp -fpc -fm=10000x10000 -ff=data/exercise7.out -ft=($tries) -fj=($jobs)" | ignore
                    nu -c $"exec ($executable) -e 7 -fp -fpr -fm=10000x10000 -ff=data/exercise7.out -ft=($tries) -fj=($jobs)" | ignore
                }
                # ##########
                print "\n"
                $jmp += 1 }
            8 => { 
                print $"(ansi purple)Running exercise 8\n(ansi purple)"
                # Exercise 8
                if not $mute {
                    nu -c $"exec ($executable) -e 8 -fs -fn=100000000 -ff=data/exercise8.out -ft=($tries) -fj=($jobs)" 
                    nu -c $"exec ($executable) -e 8 -fp -fn=100000000 -ff=data/exercise8.out -ft=($tries) -fj=($jobs)" 
                } else {
                    nu -c $"exec ($executable) -e 8 -fs -fn=100000000 -ff=data/exercise8.out -ft=($tries) -fj=($jobs)" | ignore
                    nu -c $"exec ($executable) -e 8 -fp -fn=100000000 -ff=data/exercise8.out -ft=($tries) -fj=($jobs)" | ignore
                }
                # ##########
                print "\n"
                $jmp += 1 }
            9 => { print $"(ansi purple)EXERCISE IS NOT IMPLEMENTED! Skipping...(ansi purple)"
                $jmp += 1 }
        }
    }
}