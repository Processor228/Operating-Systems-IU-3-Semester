ln ex1.txt ex12.txt
ln ex1.txt ex11.txt
# 1)
# linking ex1.txt to ex11.txt and ex12.txt, and checking the content of the file, we may
# observe that they are same.

stat ex11.txt
stat ex12.txt
# 2)
# Their inodes are also same.

du -h ex1.txt

ln ex1.txt ex13.txt
mv ex13.txt /tmp

find ./ -inum $(stat -c '%i' ex1.txt)
find / -inum $(stat -c '%i' ex1.txt)
# 3)
# difference is that in the first case the find command will scan starting from the current directory,
# and second will scan the entire file system.
# the second will find more files with this inode, just because the search space is broader.

stat ex1.txt
# 4)
# got 4 links

find ./ -inum $(stat -c '%i' ex1.txt) -exec rm {} \;
