#!/bin/bash

cd $(pwd)

job_num=2
tries=1
executable="./build/exe"
mute=false

jmp_to=1

# argument reader
for i in "$@"; do
    case $i in
    -j=* | --jobs=*)
        num='^[0-9]+$'
        if ! [[ ${i#*=} =~ $num ]]; then
            echo -e "\e[31mERROR! '--jobs' expected a number, but was given something else!\e[0m"
            exit 1
        else 
            job_num=${i#*=}
        fi
        ;;
    -t=* | --tries=*)
        num='^[0-9]+$'
        if ! [[ ${i#*=} =~ $num ]]; then
            echo -e "\e[31mERROR! '--tries' expected a number, but was given something else!\e[0m"
            exit 1
        fi
        ;;
    -e | --executable=*)
        executable=${i#*=}
        ;;
    -m | --mute)
        mute=true
        ;;
    -jmp=*)
        jmp_to=${i#*=}
        if ! [[ $jmp_to =~ $num ]]; then
            echo -e "\e[31mERROR! '--jump' expected a number, but was given something else!\e[0m"
            exit 1
        fi
        ;;
    -h | --help | *)
        echo -e "\e[33mParallel Systems Exercise 1 Execution Assistant (Bash version) -- Christoforos-Marios Mamaloukas"
        echo "Available options:"
        echo -e "\t* -h, --help: Print this message"
        echo -e "\t* -j=<number>, --jobs=<number>: Amount of processors to use in the exercise. Default is 2"
        echo -e "\t* -t=<number>, --tries=<number>: Number of runs for every exercise. Default is 1"
        echo -e "\t* -e=<executable>, --executable=<executable>: Name of the executable to run. Default is 'exe'"
        echo -e "\t* -m, --mute: Suppress all output from the exercise. Default behavior is to print output"
        echo -e "\t* -jmp=<number>: Jump to a specific exercise. Default is 1"
        exit 0
        ;; 
    esac
done

# Create data directory if it doesn't exist
if [ ! -d "./data/" ]; then
    mkdir ./data/
fi

# Remove previous output files
for i in {$jmp..9}; do
    if [ -f "./data/exercise${i}.txt" ]; then
        rm ./data/exercise${i}.txt
    fi
done

options="-fj=$job_num -ft=$tries"

case $jmp_to in
1)
    # Exercise 1
    echo -e "\e[33mExecuting Exercise 1 on $job_num threads...\e[0m"

    if [ "$mute" = false ]; then
    # Exercise 1
    # 10000 throws
        $executable -e 1 -fs -fn=10000 -ff=data/exercise1.txt $options
        $executable -e 1 -fp -fn=10000 -ff=data/exercise1.txt $options
        $executable -e 1 -fomp -fn=10000 -ff=data/exercise1.txt $options
    # 100000 throws
        $executable -e 1 -fs -fn=100000 -ff=data/exercise1.txt $options
        $executable -e 1 -fp -fn=100000 -ff=data/exercise1.txt $options
        $executable -e 1 -fomp -fn=100000 -ff=data/exercise1.txt $options
    # 1000000 throws
        $executable -e 1 -fs -fn=1000000 -ff=data/exercise1.txt $options
        $executable -e 1 -fp -fn=1000000 -ff=data/exercise1.txt $options
        $executable -e 1 -fomp -fn=1000000 -ff=data/exercise1.txt $options
    # 10000000 throws
        $executable -e 1 -fs -fn=10000000 -ff=data/exercise1.txt $options
        $executable -e 1 -fp -fn=10000000 -ff=data/exercise1.txt $options
        $executable -e 1 -fomp -fn=10000000 -ff=data/exercise1.txt $options
    # 100000000 throws
        $executable -e 1 -fs -fn=1000000000 -ff=data/exercise1.txt $options
        $executable -e 1 -fp -fn=1000000000 -ff=data/exercise1.txt $options
        $executable -e 1 -fomp -fn=1000000000 -ff=data/exercise1.txt $options
    else 
    # Exercise 1
    # 10000 throws
        $executable -e 1 -fs -fn=10000 -ff=data/exercise1.txt $options > /dev/null 2>&1
        $executable -e 1 -fp -fn=10000 -ff=data/exercise1.txt $options > /dev/null 2>&1
        $executable -e 1 -fomp -fn=10000 -ff=data/exercise1.txt $options > /dev/null 2>&1
    # 1000000 throws
        $executable -e 1 -fs -fn=1000000 -ff=data/exercise1.txt $options > /dev/null 2>&1
        $executable -e 1 -fp -fn=1000000 -ff=data/exercise1.txt $options > /dev/null 2>&1
        $executable -e 1 -fomp -fn=1000000 -ff=data/exercise1.txt $options > /dev/null 2>&1
    # 10000000 throws
        $executable -e 1 -fs -fn=10000000 -ff=data/exercise1.txt $options > /dev/null 2>&1
        $executable -e 1 -fp -fn=10000000 -ff=data/exercise1.txt $options > /dev/null 2>&1
        $executable -e 1 -fomp -fn=10000000 -ff=data/exercise1.txt $options > /dev/null 2>&1
    # 100000000 throws
        $executable -e 1 -fs -fn=1000000000 -ff=data/exercise1.txt $options > /dev/null 2>&1
        $executable -e 1 -fp -fn=1000000000 -ff=data/exercise1.txt $options > /dev/null 2>&1
        $executable -e 1 -fomp -fn=1000000000 -ff=data/exercise1.txt $options > /dev/null 2>&1
    fi
    ;&
2)
    # Exercise 2
    echo -e "\e[33mExecuting Exercise 2 on $job_num threads...\e[0m"
    # 10000 throws
    if [ "$mute" = false ]; then
        $executable -e 2 -fl -fi=10000 -ff=data/exercise2.txt $options
        $executable -e 2 -fa -fi=10000 -ff=data/exercise2.txt $options
    else 
        $executable -e 2 -fl -fi=10000 -ff=data/exercise2.txt $options > /dev/null 2>&1
        $executable -e 2 -fa -fi=10000 -ff=data/exercise2.txt $options > /dev/null 2>&1
    fi
    # 100000 throws
    if [ "$mute" = false ]; then
        $executable -e 2 -fl -fi=100000 -ff=data/exercise2.txt $options
        $executable -e 2 -fa -fi=100000 -ff=data/exercise2.txt $options
    else 
        $executable -e 2 -fl -fi=100000 -ff=data/exercise2.txt $options > /dev/null 2>&1
        $executable -e 2 -fa -fi=100000 -ff=data/exercise2.txt $options > /dev/null 2>&1
    fi
    # 1000000 throws
    if [ "$mute" = false ]; then
        $executable -e 2 -fl -fi=1000000 -ff=data/exercise2.txt $options
        $executable -e 2 -fa -fi=1000000 -ff=data/exercise2.txt $options
    else 
        $executable -e 2 -fl -fi=1000000 -ff=data/exercise2.txt $options > /dev/null 2>&1
        $executable -e 2 -fa -fi=1000000 -ff=data/exercise2.txt $options > /dev/null 2>&1
    fi
    # 10000000 throws
    if [ "$mute" = false ]; then
        $executable -e 2 -fl -fi=10000000 -ff=data/exercise2.txt $options
        $executable -e 2 -fa -fi=10000000 -ff=data/exercise2.txt $options
    else 
        $executable -e 2 -fl -fi=10000000 -ff=data/exercise2.txt $options > /dev/null 2>&1
        $executable -e 2 -fa -fi=10000000 -ff=data/exercise2.txt $options > /dev/null 2>&1
    fi
    # 100000000 throws
    if [ "$mute" = false ]; then
        $executable -e 2 -fl -fi=100000000 -ff=data/exercise2.txt $options
        $executable -e 2 -fa -fi=100000000 -ff=data/exercise2.txt $options
    else 
        $executable -e 2 -fl -fi=100000000 -ff=data/exercise2.txt $options > /dev/null 2>&1
        $executable -e 2 -fa -fi=100000000 -ff=data/exercise2.txt $options > /dev/null 2>&1
    fi
    ;&
3)
    # Exercise 3
    echo -e "\e[33mExecuting Exercise 2 on $job_num threads...\e[0m"
    # 10000 throws
    if [ "$mute" = false ]; then
        $executable -e 3 -fl -fi=10000 -ff=data/exercise3.txt $options
        $executable -e 3 -fa -fi=10000 -ff=data/exercise3.txt $options
    else 
        $executable -e 3 -fl -fi=10000 -ff=data/exercise3.txt $options > /dev/null 2>&1
        $executable -e 3 -fa -fi=10000 -ff=data/exercise3.txt $options > /dev/null 2>&1
    fi
    # 100000 throws
    if [ "$mute" = false ]; then
        $executable -e 3 -fl -fi=100000 -ff=data/exercise3.txt $options
        $executable -e 3 -fa -fi=100000 -ff=data/exercise3.txt $options
    else 
        $executable -e 3 -fl -fi=100000 -ff=data/exercise3.txt $options > /dev/null 2>&1
        $executable -e 3 -fa -fi=100000 -ff=data/exercise3.txt $options > /dev/null 2>&1
    fi
    # 1000000 throws
    if [ "$mute" = false ]; then
        $executable -e 3 -fl -fi=1000000 -ff=data/exercise3.txt $options
        $executable -e 3 -fa -fi=1000000 -ff=data/exercise3.txt $options
    else 
        $executable -e 3 -fl -fi=1000000 -ff=data/exercise3.txt $options > /dev/null 2>&1
        $executable -e 3 -fa -fi=1000000 -ff=data/exercise3.txt $options > /dev/null 2>&1
    fi
    # 10000000 throws
    if [ "$mute" = false ]; then
        $executable -e 3 -fl -fi=10000000 -ff=data/exercise3.txt $options
        $executable -e 3 -fa -fi=10000000 -ff=data/exercise3.txt $options
    else 
        $executable -e 3 -fl -fi=10000000 -ff=data/exercise3.txt $options > /dev/null 2>&1
        $executable -e 3 -fa -fi=10000000 -ff=data/exercise3.txt $options > /dev/null 2>&1
    fi
    # 100000000 throws
    if [ "$mute" = false ]; then
        $executable -e 3 -fl -fi=100000000 -ff=data/exercise3.txt $options
        $executable -e 3 -fa -fi=100000000 -ff=data/exercise3.txt $options
    else 
        $executable -e 3 -fl -fi=100000000 -ff=data/exercise3.txt $options > /dev/null 2>&1
        $executable -e 3 -fa -fi=100000000 -ff=data/exercise3.txt $options > /dev/null 2>&1
    fi
    ;&
4)
    echo -e "\e[33mExecuting Exercise 4 on $job_num threads...\e[0m"
    # 10000 loops, 1 thread
    if [ "$mute" = false ]; then
        $executable -e 4 -fp -fn=10000 -ff=data/exercise4.txt -fj=1 -ft=4
        $executable -e 4 -fc -fn=10000 -ff=data/exercise4.txt -fj=1 -ft=4
        $"$executable-dbg" -e 4 -fb -fn=10000 -ff=data/exercise4.txt -fj=1 -ft=4
    else 
        $executable -e 4 -fp -fn=10000 -ff=data/exercise4.txt -fj=1 -ft=4 > /dev/null 2>&1
        $executable -e 4 -fc -fn=10000 -ff=data/exercise4.txt -fj=1 -ft=4 > /dev/null 2>&1
        $"$executable-dbg" -e 4 -fb -fn=10000 -ff=data/exercise4.txt -fj=1 -ft=4 > /dev/null 2>&1
    fi
    # 100000 loops, 1 thread
    if [ "$mute" = false ]; then
        $executable -e 4 -fp -fn=100000 -ff=data/exercise4.txt -fj=1 -ft=4
        $executable -e 4 -fc -fn=100000 -ff=data/exercise4.txt -fj=1 -ft=4
        $"$executable-dbg" -e 4 -fb -fn=100000 -ff=data/exercise4.txt -fj=1 -ft=4
    else 
        $executable -e 4 -fp -fn=100000 -ff=data/exercise4.txt -fj=1 -ft=4 > /dev/null 2>&1
        $executable -e 4 -fc -fn=100000 -ff=data/exercise4.txt -fj=1 -ft=4 > /dev/null 2>&1
        $"$executable-dbg" -e 4 -fb -fn=100000 -ff=data/exercise4.txt -fj=1 -ft=4 > /dev/null 2>&1
    fi
    # 1000000 loops, 1 thread
    if [ "$mute" = false ]; then
        $executable -e 4 -fp -fn=1000000 -ff=data/exercise4.txt -fj=1 -ft=4
        $executable -e 4 -fc -fn=1000000 -ff=data/exercise4.txt -fj=1 -ft=4
        $"$executable-dbg" -e 4 -fb -fn=1000000 -ff=data/exercise4.txt -fj=1 -ft=4
    else 
        $executable -e 4 -fp -fn=1000000 -ff=data/exercise4.txt -fj=1 -ft=4 > /dev/null 2>&1
        $executable -e 4 -fc -fn=1000000 -ff=data/exercise4.txt -fj=1 -ft=4 > /dev/null 2>&1
        $"$executable-dbg" -e 4 -fb -fn=1000000 -ff=data/exercise4.txt -fj=1 -ft=4 > /dev/null 2>&1
    fi
    # 10000000 loops, 1 thread
    if [ "$mute" = false ]; then
        $executable -e 4 -fp -fn=10000000 -ff=data/exercise4.txt -fj=1 -ft=4
        $executable -e 4 -fc -fn=10000000 -ff=data/exercise4.txt -fj=1 -ft=4
        $"$executable-dbg" -e 4 -fb -fn=10000000 -ff=data/exercise4.txt -fj=1 -ft=4
    else 
        $executable -e 4 -fp -fn=10000000 -ff=data/exercise4.txt -fj=1 -ft=4 > /dev/null 2>&1
        $executable -e 4 -fc -fn=10000000 -ff=data/exercise4.txt -fj=1 -ft=4 > /dev/null 2>&1
        $"$executable-dbg" -e 4 -fb -fn=10000000 -ff=data/exercise4.txt -fj=1 -ft=4 > /dev/null 2>&1
    fi
    # 100000, 2 threads
    if [ "$mute" = false ]; then
        $executable -e 4 -fp -fn=100000 -ff=data/exercise4.txt -fj=2 -ft=4
        $executable -e 4 -fc -fn=100000 -ff=data/exercise4.txt -fj=2 -ft=4
        $"$executable-dbg" -e 4 -fb -fn=100000 -ff=data/exercise4.txt -fj=2 -ft=4
    else 
        $executable -e 4 -fp -fn=100000 -ff=data/exercise4.txt -fj=2 -ft=4 > /dev/null 2>&1
        $executable -e 4 -fc -fn=100000 -ff=data/exercise4.txt -fj=2 -ft=4 > /dev/null 2>&1
        $"$executable-dbg" -e 4 -fb -fn=100000 -ff=data/exercise4.txt -fj=2 -ft=4 > /dev/null 2>&1
    fi
    # 1000000, 2 threads
    if [ "$mute" = false ]; then
        $executable -e 4 -fp -fn=1000000 -ff=data/exercise4.txt -fj=2 -ft=4
        $executable -e 4 -fc -fn=1000000 -ff=data/exercise4.txt -fj=2 -ft=4
        $"$executable-dbg" -e 4 -fb -fn=1000000 -ff=data/exercise4.txt -fj=2 -ft=4
    else 
        $executable -e 4 -fp -fn=1000000 -ff=data/exercise4.txt -fj=2 -ft=4 > /dev/null 2>&1
        $executable -e 4 -fc -fn=1000000 -ff=data/exercise4.txt -fj=2 -ft=4 > /dev/null 2>&1
        $"$executable-dbg" -e 4 -fb -fn=1000000 -ff=data/exercise4.txt -fj=2 -ft=4 > /dev/null 2>&1
    fi
    # 10000000, 2 threads
    if [ "$mute" = false ]; then
        $executable -e 4 -fp -fn=10000000 -ff=data/exercise4.txt -fj=2 -ft=4
        $executable -e 4 -fc -fn=10000000 -ff=data/exercise4.txt -fj=2 -ft=4
        $"$executable-dbg" -e 4 -fb -fn=10000000 -ff=data/exercise4.txt -fj=2 -ft=4
    else 
        $executable -e 4 -fp -fn=10000000 -ff=data/exercise4.txt -fj=2 -ft=4 > /dev/null 2>&1
        $executable -e 4 -fc -fn=10000000 -ff=data/exercise4.txt -fj=2 -ft=4 > /dev/null 2>&1
        $"$executable-dbg" -e 4 -fb -fn=10000000 -ff=data/exercise4.txt -fj=2 -ft=4 > /dev/null 2>&1
    fi
    # 100000, 4 threads
    if [ "$mute" = false ]; then
        $executable -e 4 -fp -fn=100000 -ff=data/exercise4.txt -fj=4 -ft=4
        $executable -e 4 -fc -fn=100000 -ff=data/exercise4.txt -fj=4 -ft=4
        $"$executable-dbg" -e 4 -fb -fn=100000 -ff=data/exercise4.txt -fj=4 -ft=4
    else 
        $executable -e 4 -fp -fn=100000 -ff=data/exercise4.txt -fj=4 -ft=4 > /dev/null 2>&1
        $executable -e 4 -fc -fn=100000 -ff=data/exercise4.txt -fj=4 -ft=4 > /dev/null 2>&1
        $"$executable-dbg" -e 4 -fb -fn=100000 -ff=data/exercise4.txt -fj=4 -ft=4 > /dev/null 2>&1
    fi
    # 1000000, 4 threads
    if [ "$mute" = false ]; then
        $executable -e 4 -fp -fn=1000000 -ff=data/exercise4.txt -fj=4 -ft=4
        $executable -e 4 -fc -fn=1000000 -ff=data/exercise4.txt -fj=4 -ft=4
        $"$executable-dbg" -e 4 -fb -fn=1000000 -ff=data/exercise4.txt -fj=4 -ft=4
    else 
        $executable -e 4 -fp -fn=1000000 -ff=data/exercise4.txt -fj=4 -ft=4 > /dev/null 2>&1
        $executable -e 4 -fc -fn=1000000 -ff=data/exercise4.txt -fj=4 -ft=4 > /dev/null 2>&1
        $"$executable-dbg" -e 4 -fb -fn=1000000 -ff=data/exercise4.txt -fj=4 -ft=4 > /dev/null 2>&1
    fi
    # 10000000, 4 threads
    if [ "$mute" = false ]; then
        $executable -e 4 -fp -fn=10000000 -ff=data/exercise4.txt -fj=4 -ft=4
        $executable -e 4 -fc -fn=10000000 -ff=data/exercise4.txt -fj=4 -ft=4
        $"$executable-dbg" -e 4 -fb -fn=10000000 -ff=data/exercise4.txt -fj=4 -ft=4
    else 
        $executable -e 4 -fp -fn=10000000 -ff=data/exercise4.txt -fj=4 -ft=4 > /dev/null 2>&1
        $executable -e 4 -fc -fn=10000000 -ff=data/exercise4.txt -fj=4 -ft=4 > /dev/null 2>&1
        $"$executable-dbg" -e 4 -fb -fn=10000000 -ff=data/exercise4.txt -fj=4 -ft=4 > /dev/null 2>&1
    fi
    ;&
5) 
    echo -e "\e[33mExecuting Exercise 5 on $job_num threads...\e[0m"
    # 1000x1000 matrix
    if [ "$mute" = false ]; then
        $executable -e 5 -fO0 -fm=1000x1000 -fu -ff=data/exercise5.txt $options
        $executable -e 5 -fO1 -fi=1000x1000 -fu -ff=data/exercise5.txt $options
    else 
        $executable -e 5 -fO0 -fm=1000x1000 -fu -ff=data/exercise5.txt $options > /dev/null 2>&1
        $executable -e 5 -fO1 -fi=1000x1000 -fu -ff=data/exercise5.txt $options > /dev/null 2>&1
    fi
    # 10000x10000 matrix
    if [ "$mute" = false ]; then
        $executable -e 5 -fO0 -fm=10000x10000 -fu -ff=data/exercise5.txt $options
        $executable -e 5 -fO1 -fi=10000x10000 -fu -ff=data/exercise5.txt $options
    else 
        $executable -e 5 -fO0 -fm=10000x10000 -fu -ff=data/exercise5.txt $options > /dev/null 2>&1
        $executable -e 5 -fO1 -fi=10000x10000 -fu -ff=data/exercise5.txt $options > /dev/null 2>&1
    fi
    # 100000x100000 matrix
    if [ "$mute" = false ]; then
        $executable -e 5 -fO0 -fm=100000x100000 -fu -ff=data/exercise5.txt $options
        $executable -e 5 -fO1 -fi=100000x100000 -fu -ff=data/exercise5.txt $options
    else 
        $executable -e 5 -fO0 -fm=100000x100000 -fu -ff=data/exercise5.txt $options > /dev/null 2>&1
        $executable -e 5 -fO1 -fi=100000x100000 -fu -ff=data/exercise5.txt $options > /dev/null 2>&1
    fi
    # 1000000x1000000 matrix
    if [ "$mute" = false ]; then
        $executable -e 5 -fO0 -fm=1000000x1000000 -fu -ff=data/exercise5.txt $options
        $executable -e 5 -fO1 -fi=1000000x1000000 -fu -ff=data/exercise5.txt $options
    else 
        $executable -e 5 -fO0 -fm=1000000x1000000 -fu -ff=data/exercise5.txt $options > /dev/null 2>&1
        $executable -e 5 -fO1 -fi=1000000x1000000 -fu -ff=data/exercise5.txt $options > /dev/null 2>&1
    fi
    ;&
6)
    echo -e "\e[33mExecuting Exercise 6 on $job_num threads...\e[0m"
    # 64x64 matrix, 1000 generations
    if [ "$mute" = false ]; then
        $executable -e 6 -fs -fm=64x64 -fg=1000 -ff=data/exercise6.txt $options
        $executable -e 6 -fp -fm=64x64 -fg=1000 -ff=data/exercise6.txt $options
    else 
        $executable -e 6 -fs -fm=64x64 -fg=1000 -ff=data/exercise6.txt $options > /dev/null 2>&1
        $executable -e 6 -fp -fi=64x64 -fg=1000 -ff=data/exercise6.txt $options > /dev/null 2>&1
    fi
    # 1024x1024 matrix, 1000 generations
    if [ "$mute" = false ]; then
        $executable -e 6 -fs -fm=1024x1024 -fg=1000 -ff=data/exercise6.txt $options
        $executable -e 6 -fp -fi=1024x1024 -fg=1000 -ff=data/exercise6.txt $options
    else 
        $executable -e 6 -fs -fm=1024x1024 -fg=1000 -ff=data/exercise6.txt $options > /dev/null 2>&1
        $executable -e 6 -fp -fi=1024x1024 -fg=1000 -ff=data/exercise6.txt $options > /dev/null 2>&1
    fi
    # 2048x2048 matrix, 1000 generations
    if [ "$mute" = false ]; then
        $executable -e 6 -fs -fm=2048x2048 -fg=1000 -ff=data/exercise6.txt $options
        $executable -e 6 -fp -fi=2048x2048 -fg=1000 -ff=data/exercise6.txt $options
    else 
        $executable -e 6 -fs -fm=2048x2048 -fg=1000 -ff=data/exercise6.txt $options > /dev/null 2>&1
        $executable -e 6 -fp -fi=2048x2048 -fg=1000 -ff=data/exercise6.txt $options > /dev/null 2>&1
    fi
    # 4096x4096 matrix, 1000 generations
    if [ "$mute" = false ]; then
        $executable -e 6 -fs -fm=4096x4096 -fg=1000 -ff=data/exercise6.txt $options
        $executable -e 6 -fp -fi=4096x4096 -fg=1000 -ff=data/exercise6.txt $options
    else 
        $executable -e 6 -fs -fm=4096x4096 -fg=1000 -ff=data/exercise6.txt $options > /dev/null 2>&1
        $executable -e 6 -fp -fi=4096x4096 -fg=1000 -ff=data/exercise6.txt $options > /dev/null 2>&1
    fi
    ;&
7) 
    echo -e "\e[33mExecuting Exercise 7 on $job_num threads...\e[0m"
    # 64x64 matrix
    if [ "$mute" = false ]; then
        $executable -e 7 -fs -fpc -fm=64 -ff=data/exercise7.txt $options
        $executable -e 7 -fs -fpr -fm=64 -ff=data/exercise7.txt $options
        $executable -e 7 -fp -fpc -fm=64 -ff=data/exercise7.txt $options
        $executable -e 7 -fp -fpr -fm=64 -ff=data/exercise7.txt $options
    else 
        $executable -e 7 -fs -fpc -fm=64 -ff=data/exercise7.txt $options > /dev/null 2>&1
        $executable -e 7 -fs -fpr -fm=64 -ff=data/exercise7.txt $options > /dev/null 2>&1
        $executable -e 7 -fp -fpc -fm=64 -ff=data/exercise7.txt $options > /dev/null 2>&1
        $executable -e 7 -fp -fpr -fm=64 -ff=data/exercise7.txt $options > /dev/null 2>&1
    fi
    # 1024x1024 matrix
    if [ "$mute" = false ]; then
        $executable -e 7 -fs -fpc -fm=1024 -ff=data/exercise7.txt $options
        $executable -e 7 -fs -fpr -fm=1024 -ff=data/exercise7.txt $options
        $executable -e 7 -fp -fpc -fm=1024 -ff=data/exercise7.txt $options
        $executable -e 7 -fp -fpr -fm=1024 -ff=data/exercise7.txt $options
    else 
        $executable -e 7 -fs -fpc -fm=1024 -ff=data/exercise7.txt $options > /dev/null 2>&1
        $executable -e 7 -fs -fpr -fm=1024 -ff=data/exercise7.txt $options > /dev/null 2>&1
        $executable -e 7 -fp -fpc -fm=1024 -ff=data/exercise7.txt $options > /dev/null 2>&1
        $executable -e 7 -fp -fpr -fm=1024 -ff=data/exercise7.txt $options > /dev/null 2>&1
    fi
    # 2048x2048 matrix
    if [ "$mute" = false ]; then
        $executable -e 7 -fs -fpc -fm=2048 -ff=data/exercise7.txt $options
        $executable -e 7 -fs -fpr -fm=2048 -ff=data/exercise7.txt $options
        $executable -e 7 -fp -fpc -fm=2048 -ff=data/exercise7.txt $options
        $executable -e 7 -fp -fpr -fm=2048 -ff=data/exercise7.txt $options
    else 
        $executable -e 7 -fs -fpc -fm=2048 -ff=data/exercise7.txt $options > /dev/null 2>&1
        $executable -e 7 -fs -fpr -fm=2048 -ff=data/exercise7.txt $options > /dev/null 2>&1
        $executable -e 7 -fp -fpc -fm=2048 -ff=data/exercise7.txt $options > /dev/null 2>&1
        $executable -e 7 -fp -fpr -fm=2048 -ff=data/exercise7.txt $options > /dev/null 2>&1
    fi
    # 4096x4096 matrix
    if [ "$mute" = false ]; then
        $executable -e 7 -fs -fpc -fm=4096 -ff=data/exercise7.txt $options
        $executable -e 7 -fs -fpr -fm=4096 -ff=data/exercise7.txt $options
        $executable -e 7 -fp -fpc -fm=4096 -ff=data/exercise7.txt $options
        $executable -e 7 -fp -fpr -fm=4096 -ff=data/exercise7.txt $options
    else 
        $executable -e 7 -fs -fpc -fm=4096 -ff=data/exercise7.txt $options > /dev/null 2>&1
        $executable -e 7 -fs -fpr -fm=4096 -ff=data/exercise7.txt $options > /dev/null 2>&1
        $executable -e 7 -fp -fpc -fm=4096 -ff=data/exercise7.txt $options > /dev/null 2>&1
        $executable -e 7 -fp -fpr -fm=4096 -ff=data/exercise7.txt $options > /dev/null 2>&1
    fi
    ;&
8) 
    echo -e "\e[33mExecuting Exercise 8 on $job_num threads...\e[0m"
    # 10000 array
    if [ "$mute" = false ]; then
        $executable -e 8 -fs -fn=10000 -ff=data/exercise8.txt $options
        $executable -e 8 -fp -fn=10000 -ff=data/exercise8.txt $options
    else 
        $executable -e 8 -fs -fn=10000 -ff=data/exercise8.txt $options > /dev/null 2>&1
        $executable -e 8 -fp -fn=10000 -ff=data/exercise8.txt $options > /dev/null 2>&1
    fi
    # 100000 array
    if [ "$mute" = false ]; then
        $executable -e 8 -fs -fn=100000 -ff=data/exercise8.txt $options
        $executable -e 8 -fp -fn=100000 -ff=data/exercise8.txt $options
    else 
        $executable -e 8 -fs -fn=100000 -ff=data/exercise8.txt $options > /dev/null 2>&1
        $executable -e 8 -fp -fn=100000 -ff=data/exercise8.txt $options > /dev/null 2>&1
    fi
    # 1000000 array
    if [ "$mute" = false ]; then
        $executable -e 8 -fs -fn=1000000 -ff=data/exercise8.txt $options
        $executable -e 8 -fp -fn=1000000 -ff=data/exercise8.txt $options
    else 
        $executable -e 8 -fs -fn=1000000 -ff=data/exercise8.txt $options > /dev/null 2>&1
        $executable -e 8 -fp -fn=1000000 -ff=data/exercise8.txt $options > /dev/null 2>&1
    fi
    # 10000000 array
    if [ "$mute" = false ]; then
        $executable -e 8 -fs -fn=10000000 -ff=data/exercise8.txt $options
        $executable -e 8 -fp -fn=10000000 -ff=data/exercise8.txt $options
    else 
        $executable -e 8 -fs -fn=10000000 -ff=data/exercise8.txt $options > /dev/null 2>&1
        $executable -e 8 -fp -fn=10000000 -ff=data/exercise8.txt $options > /dev/null 2>&1
    fi
    # 100000000 array
    if [ "$mute" = false ]; then
        $executable -e 8 -fs -fn=100000000 -ff=data/exercise8.txt $options
        $executable -e 8 -fp -fn=100000000 -ff=data/exercise8.txt $options
    else 
        $executable -e 8 -fs -fn=100000000 -ff=data/exercise8.txt $options > /dev/null 2>&1
        $executable -e 8 -fp -fn=100000000 -ff=data/exercise8.txt $options > /dev/null 2>&1
    fi
    ;;
*)
    echo -e "\e[31mERROR! Invalid exercise number!\e[0m"
    exit 1
    ;;
esac

echo -e "\e[0m"