touch ex5.txt

# remove write permission for everybody
chmod o-w ex5.txt

# grant all permissions to owner and others
chmod 770 ex5.txt

# make group permissions equal to user permissions
chmod g=u ex5.txt

# What does 660 mean for ex5.txt? user rw- group rw- others ---
# What does 775 mean for ex5.txt? user rwx group rwx others r-x
# What does 777 mean for ex5.txt? all permissions for everyone