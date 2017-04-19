#!/bin/bash

mkdir -p build
cd build
cmake ../cdn
make
cd ../bin

for CASE_ID in `seq 0 9`;
do
    mkdir -p 1Intermediate
    ./cdn ../../case_example/batch3/1Intermediate/case${CASE_ID}.txt 1Intermediate/result${CASE_ID}.txt
    mkdir -p 2Advanced
    ./cdn ../../case_example/batch3/2Advanced/case${CASE_ID}.txt 2Advanced/result${CASE_ID}.txt
done
