#!/bin/bash
gcc ex1.c -o ex1 # compile the file

# creating the file not less than 50 MB
dd if=/dev/zero of=lofs.img  bs=50M  count=1

# Setup a loop device on the created file, using losetup
loop=$(sudo losetup --find --show lofs.img)

mkfs lofs.img # created a loop file system

# mounting filesystem onto the mount point lofsdisk
mkdir lofsdisk
sudo mount lofs.img lofsdisk/ # mounting the filesystem into lofsdisk
sudo chmod 777 lofsdisk/ # adding permission to open and change contents of the directory

# get inside the lofsdisk
cd lofsdisk/ || (echo "couldn't enter lofsdisk" && exit)

# Add two files file1, file2 to the LOFS where file1 contains your
# first name, and file2 contains your last name
echo "max" > file1
echo "kamenetskii" > file2

cp ../ex1 ex1

declare -a output_array

# Appends all the file paths into the "output_array"
function get_libs() {
    local binary=$1
    echo "collecting shared libs of $binary..."
    while IFS= read -r line; do # reading all the output of ldd, if "=>" is present in line, because it is the sign
      output_array+=( "$line" ) # that this file is a shared library, not a linker of anything else.
      done < <(
    ldd "$binary" | grep "=> /" | awk '{print $3}' | cut -c 2-)
}

# here we parse the dynamic linker separately from the libraries, by the same way.
function get_linker() {
    local binary=$1
    echo "Collecting linker of $binary..."
    while IFS= read -r line; do
          output_array+=( "$line" )
          done < <(
        ldd "$binary" | tail -1 | awk '{print $1}' | cut -c 2-) # the linker is always the last listed entry.
}

# function to add a binary and discover all it's dependencies. Provide ONE argument, like: add_binary /bin/cat
function add_binary() {
    local binary=$(echo "$1" | cut -c 2-)
    mkdir -p "${binary%/*}" && touch "$binary"  # copying the same file structure, then adding the file itself there
    chmod +x "$binary" # adding the execution permission
    cp "/$binary" "$binary" # copying the content into the file
    get_libs "/$binary" # issuing all the libs of the provided binary
    get_linker "/$binary"
}


# adding libs of binaries that were asked in the exercise
add_binary /bin/bash
add_binary /bin/cat
add_binary /bin/echo
add_binary /bin/ls

# before running any executable, we need to install linker into the system, which is /lib64/ld-linux-x86-64.so.2
# in my case. this is not a shared library, this is a linker. So it SHOULD NOT be installed by get_libs().

for file in "${output_array[@]}" # iterating through all the dependencies
do
  mkdir -p "${file%/*}" && touch "$file" # and creating the same file structure
  cp "/$file" "$file" # copying the binaries contents
  chmod +x "$file"  # adding the execution permission
  echo "installing file $file..."
done

chmod +x ex1  # making it executable
{ echo "with chroot"; (sudo chroot ./ /ex1); }>> ex1.txt # executing the ex1 with a changed root
{ echo "no chroot"; ./ex1; } >> ex1.txt # run it without changing the root
cp ex1.txt ../ex1.txt # copy the results into the week11 directory
