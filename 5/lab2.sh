#!/bin/bash

if [ $# -ge 1 ]
then
    path=$1
    shift
fi

while ! [ -e "$path" -a -d "$path" -a -r "$path" -a -w "$path" -a -x "$path" ]
# r - to obviously read the files in the directory
# w - to add the object files in the same directory
# x - apparently can't read from the directory if you can't open it(that's what execution rights are for in this case)
do
    read -p "Given directory ($path) either does not exist, is not a directory, its files can't be read/modified or it can't be accessed. Insert a new directory here: " path
done

for file in "$path"/*
do
    if [ -r "$file" ] && [[ "$file" =~ ^.*\.cpp$ ]]
    # r - to obviously read teh source file(how else will the compiler compile)
    #           (could just not include this option and call chmod on the output file but...)
    then
        g++ "$file" -o "$(dirname "$file")/$(basename "$file" .cpp)" -Wall
    fi
done
