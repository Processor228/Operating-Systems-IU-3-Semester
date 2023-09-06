mkdir root
mkdir home

date && sleep 3
ls / -t --reverse > root/root.txt

date && sleep 3
ls ~ -t --reverse > home/home.txt

cat root/root.txt
cat home/home.txt

ls root
ls home
