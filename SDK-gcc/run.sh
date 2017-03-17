#!/bin/bash

mkdir -p build
cd build
cmake ../cdn
make
cd ../bin
./cdn ../../case_example/case0.txt  result0.txt
./cdn ../../case_example/case1.txt  result1.txt
./cdn ../../case_example/case2.txt  result2.txt
./cdn ../../case_example/case3.txt  result3.txt
./cdn ../../case_example/case4.txt  result4.txt
