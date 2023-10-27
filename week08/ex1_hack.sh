#!/bin/bash
gcc ex1.c -o ex1
./ex1&

sleep 1 # so ex1 really writes it's pid to the file

read -r pid < /tmp/ex1.pid
echo "process id: $pid"

# start searching for password...
heap_info=$(grep 'heap' /proc/"$pid"/maps)
echo "$heap_info"

start=$(echo "$heap_info" | cut -d "-" -f1)
end=$(echo "$heap_info" | cut -d "-" -f2 | cut -d " " -f1)

length=$(("0x$end" - "0x$start"))

pass=$(sudo xxd -s "0x$start" -l "$length" /proc/"$pid"/mem | less | grep "pass:")

addr=$(echo "$pass" | cut -d ":" -f1)

res=$(echo "$pass" | awk -F'pass:' '{print $2}' | cut -d "." -f1)
echo "Res: $res address is 0x$addr"

kill "$pid"