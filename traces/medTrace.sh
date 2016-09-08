#!/bin/bash

for i in `seq 1 30`; do
    mktest 75 37 10 > medio/arq$i.in
    sleep 1
done 
