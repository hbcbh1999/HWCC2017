echo "batch3/1Intermediate"
for CASE_ID in `seq 0 9`;
do
    python check.py case_example/batch3/1Intermediate/case${CASE_ID}.txt SDK-gcc/bin/1Intermediate/result${CASE_ID}.txt
done
echo "batch3/2Advanced"
for CASE_ID in `seq 0 9`;
do
    python check.py case_example/batch3/2Advanced/case${CASE_ID}.txt SDK-gcc/bin/2Advanced/result${CASE_ID}.txt
done
