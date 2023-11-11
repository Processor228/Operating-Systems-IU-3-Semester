gcc monitor.c -o monitor
gcc ex1.c -o ex1

path="./"

gnome-terminal -- ./monitor "$path"
./ex1 "$path"

#bash ex1_test.sh

rm monitor
rm ex1

