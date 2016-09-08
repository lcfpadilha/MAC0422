#!/bin/bash

for i in `seq 1 30`; do
    ep1 1 traces/grande/arq$i.in results/grande/FCFS/arq$i.out y > results/grande/FCFS/stdout$i.txt 2> results/grande/FCFS/err$i.txt && cat results/grande/FCFS/stdout$i.txt
done 