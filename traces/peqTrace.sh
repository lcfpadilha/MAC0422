#!/bin/bash

for i in `seq 1 30`; do
	mktest 10 5 10 > pequeno/arq$i.in
    sleep 1
done 
