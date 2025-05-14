#!/usr/bin/bash

cd $(pwd)

job_num=2
executable="./build/exe"
mute=false

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
    -e | --executable=*)
        executable=${i#*=}
        ;;
    -m | --mute)
        mute=true
        ;;
    -h | --help | *)
        echo -e "\e[33mParallel Systems Exercise 1 Execution Assistant (Bash version) -- Christoforos-Marios Mamaloukas"
        echo "Available options:"
        echo -e "\t* -h, --help: Print this message"
        echo -e "\t* -j=<number>, --jobs=<number>: Amount of processors to use in the exercise. Default is 2"
        echo -e "\t* -e=<executable>, --executable=<executable>: Name of the executable to run. Default is 'exe'"
        echo -e "\t* -m, --mute: Suppress all output from the exercise. Default behavior is to print output"
        echo -e "\t* -m, --no-weeb: Do not use Ninja as a build system\e[0m"
        exit 0
        ;; 
    esac
done

# Create data directory if it doesn't exist
if [ ! -d "./data/" ]; then
    mkdir ./data/
fi

# Remove previous output files
for i in {1..9}; do
    if [ -f "./data/exercise${i}.txt" ]; then
        rm ./data/exercise${i}.txt
    fi
done

# Exercise 1
echo -e "\e[33mExecuting Exercise 1 on $job_num threads...\e[0m"

if [ "$mute" = false ]; then
    $executable -e 1 -fs -fj=$job_num -fn=1000000000 -ff=data/exercise1.txt 
    $executable -e 1 -fp -fj=$job_num -fn=1000000000 -ff=data/exercise1.txt 
    $executable -e 1 -fomp -fj=$job_num -fn=1000000000 -ff=data/exercise1.txt 
else 
    $executable -e 1 -fs -fj=$job_num -fn=1000000000 -ff=data/exercise1.txt > /dev/null 2>&1
    $executable -e 1 -fp -fj=$job_num -fn=1000000000 -ff=data/exercise1.txt > /dev/null 2>&1
    $executable -e 1 -fomp -fj=$job_num -fn=1000000000 -ff=data/exercise1.txt > /dev/null 2>&1
fi

# Exercise 2
echo -e "\e[33mExecuting Exercise 2 on $job_num threads...\e[0m"

if [ "$mute" = false ]; then
    $executable -e 2 -fl -fj=$job_num -fi=1000000000 -ff=data/exercise2.txt 
    $executable -e 2 -fa -fj=$job_num -fi=1000000000 -ff=data/exercise2.txt 
else 
    $executable -e 2 -fl -fj=$job_num -fi=1000000000 -ff=data/exercise2.txt > /dev/null 2>&1
    $executable -e 2 -fa -fj=$job_num -fi=1000000000 -ff=data/exercise2.txt > /dev/null 2>&1
fi

echo -e "\e[0m"