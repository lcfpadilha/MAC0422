#!/bin/bash

for i in `seq 1 30`; do
    mktest 150 75 10 > grande/arq$i.in
    sleep 1
done 
