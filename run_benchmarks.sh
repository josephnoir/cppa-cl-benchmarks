#!/bin/bash

size=1000
from=1000
to=10000
step=1000
measurements=50

usage="\
Usage: $0
    --size          matrix size (default: $size)
    --from          start of iterations (default: $from)
    --to            end of iterations (deafult: $to)
    --step          iteration increment (default: $step)
    --measurements  measurements per iteration (default: $measurements)
"

#
# $1: program to run
# $2: iterations in that program
#
function run {
    for measurement in $(seq 1 $measurements) ; do
        if [[ $(uname) == "Darwin" ]] ; then
            /usr/bin/time -p $1 -s $size -i $2 2>&1 | awk '/real/ {print $2}'
        else
            /usr/bin/time -p -f "%e" $1 -s $size -i $2 2>&1
        fi
    done
}

while [ $# -ne 0 ]; do
    case "$1" in
        -*=*) optarg=`echo "$1" | sed 's/[-_a-zA-Z0-9]*=//'` ;;
        *) optarg= ;;
    esac

    case "$1" in
        --help|-h)
            echo "${usage}" 1>&2
            exit 1
            ;;
        --step=*)
            step=$optarg
            ;;
        --from=*)
            from=$optarg
            ;;
        --to=*)
            to=$optarg
            ;;
        --size=*)
            size=$optarg
            ;;
        --measurements=*)
            measurements=$optarg
            ;;
        *)
            echo "Invalid option '$1'.  Try $0 --help to see available options."
            exit 1
            ;;
    esac
    shift
done

# opencl_cppa opencl_native copy_ops_small spawn_time spawn_time_core
for i in copy_ops_small 
do
    for iteration in $(seq $from $step $to); do
        next_file="${iteration}_${i}.txt"
        rm -f $next_file
        echo "$next_file"
        run build/bin/$i $iteration >> $next_file
        echo "" >> $next_file
    done
done


