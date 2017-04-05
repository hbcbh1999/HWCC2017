#!/bin/bash

mkdir -p build
cd build
cmake ../cdn
make
cd ../bin
./cdn ../../case_example/case0.txt result0.txt
./cdn ../../case_example/case1.txt result1.txt
./cdn ../../case_example/case2.txt result2.txt
./cdn ../../case_example/case3.txt result3.txt
./cdn ../../case_example/case4.txt result4.txt

for CASE_ID in `seq 0 8`;
do
    mkdir -p 0Primary
    ./cdn ../../case_example/0Primary/case${CASE_ID}.txt 0Primary/result${CASE_ID}.txt
    mkdir -p 1Intermediate
    ./cdn ../../case_example/1Intermediate/case${CASE_ID}.txt 1Intermediate/result${CASE_ID}.txt
    mkdir -p 2Advanced
    ./cdn ../../case_example/2Advanced/case${CASE_ID}.txt 2Advanced/result${CASE_ID}.txt
done
