rm ./tmp

#mkdir tmp

ln -s ./tmp tmp1
ls -li

mkdir rmp

ls -li
#1)
# The difference between the two ls -li outputs is that after creating the ./tmp directory, the
# ./tmp1 symbolic link now points to a valid directory, whereas before the directory didn't exist.

bash gen.sh 10 ex1.txt
mv ex1.txt ./tmp

ls -li /tmp1

ln -s tmp ./tmp1/tmp2

./gen.sh 10 ex1.txt
mv ex1.txt ./tmp1/tmp2

ls -li ./tmp1/tmp2

rm -rf ./tmp
ls -li ./tmp1
ls -li ./tmp1/tmp2

rm ./tmp1
rm ./tmp1/tmp2

