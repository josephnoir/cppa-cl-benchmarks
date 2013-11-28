#!/bin/bash

#
# file have to be named <size>_<bench>.txt
# outputs to <bench>.dat
#

for bench in "opencl_cppa" "opencl_native" "copy_ops_small_quadro" "copy_ops_small_quadro_2" "copy_ops_small_tesla" "spawn_time" "spawn_time_core" "copy_ops_small_9300MGS" "copy_ops_small_RadeonHD6970M" "overhead_diff"; do
#for bench in "spawn_time" "spawn_time_core"; do
#for bench in "overhead_diff"; do
    fname="$bench.dat"
    rm -f "$fname"
    #for size in $(seq -f "%1.f" 100000 100000 1000000); do
    for size in $(seq 1000 1000 10000); do
        line="$size"
        in_file="${size}_${bench}.txt"
        echo "$in_file"
        #stats=$(./statistics.sh "$in_file" 2>/dev/null)
        stats=$(perl data2stat.pl "$in_file" 2>/dev/null)
        line="$line, $stats"
        echo "$line" >> "$fname"
    done
done


