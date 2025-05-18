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
    echo -e "\e[33mExecuting Exercise 3 on $job_num threads...\e[0m"
    if [ "$mute" = false ]; then
        $executable -e 3 -fl -fi=100000000 -ff=data/exercise2.txt $options
        $executable -e 3 -fa -fi=100000000 -ff=data/exercise2.txt $options
    else 
        $executable -e 3 -fl -fi=100000000 -ff=data/exercise2.txt $options > /dev/null 2>&1
        $executable -e 3 -fa -fi=100000000 -ff=data/exercise2.txt $options > /dev/null 2>&1
    fi
    ;;
*)
    echo -e "\e[31mERROR! Invalid exercise number!\e[0m"
    exit 1
    ;;
esac

echo -e "\e[0m"