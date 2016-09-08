#!/bin/bash

for i in `seq 1 30`; do
    ep1 2 traces/pequeno/arq$i.in results/pequeno/SRTN/arq$i.out y > results/pequeno/SRTN/stdout$i.txt 2> results/pequeno/SRTN/err$i.txt
done 

for i in `seq 1 30`; do
    ep1 2 traces/medio/arq$i.in results/medio/SRTN/arq$i.out y > results/medio/SRTN/stdout$i.txt 2> results/medio/SRTN/err$i.txt
done 

for i in `seq 1 30`; do
    ep1 2 traces/grande/arq$i.in results/grande/SRTN/arq$i.out y > results/grande/SRTN/stdout$i.txt 2> results/grande/SRTN/err$i.txt
done 
