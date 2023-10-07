gcc ex3.c -o ex3 -pthread -lm

{ time ./ex3  10000000 1; } 2> ex3_res.txt

{ time ./ex3  10000000 2; } 2>> ex3_res.txt

{ time ./ex3  10000000 4; } 2>> ex3_res.txt

{ time ./ex3  10000000 10; } 2>> ex3_res.txt

{ time ./ex3  10000000 100; } 2>> ex3_res.txt


