stat ex1

cp ex1 ex2

stat ex2 | grep Links
# no, they do not have the same inode numbers, because cp command just copies the contents in
# this case.

stat -c "%h-%n" /etc/* | grep ^3
# these links represent how many files in system point to this inode
