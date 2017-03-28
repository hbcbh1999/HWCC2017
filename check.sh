# python check.py case_example/case0.txt SDK-gcc/bin/result0.txt
# python check.py case_example/case1.txt SDK-gcc/bin/result1.txt
# python check.py case_example/case2.txt SDK-gcc/bin/result2.txt
# python check.py case_example/case3.txt SDK-gcc/bin/result3.txt
# python check.py case_example/case4.txt SDK-gcc/bin/result4.txt

CASE_ID=0
python check.py case_example/0Primary/case${CASE_ID}.txt SDK-gcc/bin/0Primary/result${CASE_ID}.txt
python check.py case_example/1Intermediate/case${CASE_ID}.txt SDK-gcc/bin/1Intermediate/result${CASE_ID}.txt
python check.py case_example/2Advanced/case${CASE_ID}.txt SDK-gcc/bin/2Advanced/result${CASE_ID}.txt
