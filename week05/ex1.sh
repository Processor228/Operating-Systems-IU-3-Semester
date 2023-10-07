#!/bin/bash
gcc publisher.c -o pub
gcc subscriber.c -o sub

gnome-terminal -- ./pub 1

# Store the first argument in a variable
end_number=$1

# Iterate from 1 to the end number
for i in $(seq 1 "$end_number")
do
    echo "subscriber $i"
    gnome-terminal -- ./sub 1
done
