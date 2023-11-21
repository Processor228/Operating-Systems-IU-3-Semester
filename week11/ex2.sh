gcc create_fs.c -o create_fs
gcc ex2.c -o implementation

./create_fs disk0

./implementation $(cat input.txt)
