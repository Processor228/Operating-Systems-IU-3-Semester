gcc ex3.c -o ex3

# n=3
./ex3 3 &

sleep 5
pstree | grep ex3
sleep 5
pstree | grep ex3
sleep 5
pstree | grep ex3


sleep 5

# n=5
./ex3 5 &
sleep 5
pstree | grep ex3

sleep 5
pstree | grep ex3

sleep 5
pstree | grep ex3

sleep 5
pstree | grep ex3

sleep 5
pstree | grep ex3

