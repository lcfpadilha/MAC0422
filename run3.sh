#!/bin/bash

for i in `seq 1 30`; do
    ./ep1 3 traces/pequeno/arq$i.in results/pequeno/MFBQ/arq$i.out y > results/pequeno/MFBQ/stdout$i.txt 2> results/pequeno/MFBQ/err$i.txt
done 

for i in `seq 1 30`; do
    ./ep1 3 traces/medio/arq$i.in results/medio/MFBQ/arq$i.out y > results/medio/MFBQ/stdout$i.txt 2> results/medio/MFBQ/err$i.txt
done 

for i in `seq 1 30`; do
    ./ep1 3 traces/grande/arq$i.in results/grande/MFBQ/arq$i.out y > results/grande/MFBQ/stdout$i.txt 2> results/grande/MFBQ/err$i.txt
done 
