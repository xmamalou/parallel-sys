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
        rm "./data/exercise${i}.txt"
    fi
done

options="-fj=$job_num -ft=$tries"

case $jmp_to in
1)
    # Exercise 1
    echo -e "\e[33mExecuting Exercise 1...\e[0m"

    # Exercise 1
    for throws in 10000 100000 1000000 10000000 100000000; do
        $executable -e 1 -fs -fn=$throws -ff=data/exercise1.txt -fj=$jobs -ft=4
    done
    for jobs in {1..8}; do
        for throws in 10000 100000 1000000 10000000 100000000; do
            $executable -e 1 -fp -fn=$throws -ff=data/exercise1.txt -fj=$jobs -ft=4
            $executable -e 1 -fomp -fn=$throws -ff=data/exercise1.txt -fj=$jobs -ft=4
        done
    done
    ;&
2)
    # Exercise 2
    echo -e "\e[33mExecuting Exercise 2...\e[0m"

    for jobs in {1..8}; do
        for throws in 10000 100000 1000000 10000000 100000000; do
            $executable -e 2 -fl -fi=$throws -ff=data/exercise2.txt -fj=$jobs -ft=4
            $executable -e 2 -fa -fi=$throws -ff=data/exercise2.txt -fj=$jobs -ft=4
        done
    done
    ;&
3)
    # Exercise 3
    echo -e "\e[33mExecuting Exercise 3...\e[0m"
    for jobs in {1..8}; do
        for throws in 10000 100000 1000000 10000000 100000000; do
            $executable -e 3 -fl -fi=$throws -ff=data/exercise3.txt -fj=$jobs -ft=4
            $executable -e 3 -fa -fi=$throws -ff=data/exercise3.txt -fj=$jobs -ft=4
            $executable -e 3 -fns -fi=$throws -ff=data/exercise3.txt -fj=$jobs -ft=4 || echo "SYNCHRONIZATION IS NECESSARY" > ./data/exercise3_error.txt
        done
    done
    ;&
4)
    echo -e "\e[33mExecuting Exercise 4...\e[0m"
    for jobs in {1..8}; do
        for throws in 1000 10000 100000 1000000; do
            $executable -e 4 -fp -fn=$throws -ff=data/exercise4.txt -fj=$jobs -ft=4
            $executable -e 4 -fc -fn=$throws -ff=data/exercise4.txt -fj=$jobs -ft=4
            $"$executable-dbg" -e 4 -fb -fn=$throws -ff=data/exercise4.txt -fj=$jobs -ft=4
        done
    done
    ;&
5) 
    echo -e "\e[33mExecuting Exercise 5...\e[0m"
    for jobs in {1..8}; do
        for throws in 10000 100000 1000000 10000000 100000000; do
            $executable -e 5 -fO0 -fm=$"$throws"x$"$throws" -fu -ff=data/exercise5.txt -fj=$jobs -ft=4
            $executable -e 5 -fO1 -fm=$"$throws"x$"$throws" -fu -ff=data/exercise5.txt -fj=$jobs -ft=4
        done
    done
    ;&
6)
    echo -e "\e[33mExecuting Exercise 6...\e[0m"
    
    for matrix in 64 128 256 512 1024; do
        $executable -e 6 -fs -fm=$matrix -fg=1000 -ff=data/exercise6.txt -fj=$jobs -ft=4
    done
    for jobs in {1..8}; do
        for matrix in 64 128 256 512 1024; do
            $executable -e 6 -fp -fm=$matrix -fg=1000 -ff=data/exercise6.txt -fj=$jobs -ft=4
        done
    done
    ;&
7) 
    echo -e "\e[33mExecuting Exercise 7...\e[0m"

    for matrix in 1024 2048 4096 8192 16384; do
        $executable -e 7 -fs -fp -fpr -fm=$matrix -ff=data/exercise7.txt -fj=$jobs -ft=4
        $executable -e 7 -fs -fp -fpc -fm=$matrix -ff=data/exercise7.txt -fj=$jobs -ft=4
    done

    for jobs in {1..8}; do
        for matrix in 1024 2048 4096 8192 16384; do
            $executable -e 7 -fp -fpr -fm=$matrix -ff=data/exercise7.txt -fj=$jobs -ft=4
            $executable -e 7 -fp -fpc -fm=$matrix -ff=data/exercise7.txt -fj=$jobs -ft=4
        done
    done
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