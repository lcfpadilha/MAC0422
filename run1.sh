#!/bin/bash

for i in `seq 1 30`; do
    ./ep1 1 traces/pequeno/arq$i.in results/pequeno/FCFS/arq$i.out y > results/pequeno/FCFS/stdout$i.txt 2> results/pequeno/FCFS/err$i.txt
done 

for i in `seq 1 30`; do
    ./ep1 1 traces/medio/arq$i.in results/medio/FCFS/arq$i.out y > results/medio/FCFS/stdout$i.txt 2> results/medio/FCFS/err$i.txt
done 

for i in `seq 1 30`; do
    ./ep1 1 traces/grande/arq$i.in results/grande/FCFS/arq$i.out y > results/grande/FCFS/stdout$i.txt 2> results/grande/FCFS/err$i.txt
done 
