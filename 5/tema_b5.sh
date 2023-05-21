#!/bin/bash

if [ $# -ge 1 ]
then
    path=$1
    shift
fi

while ! [ -e "$path" -a -d "$path" -a -r "$path" -a -x "$path" ]
# r - to read the files from the directoy
# x - to access its contents
do
    read -p "Given directory ($path) either does not exist, is not a directory, its files can't be read or it can't be accessed. Insert a new directory here: " path
done

archives="$(dirname "$0")/archives"

mkdir "$archives"

for name in "$path"/*.txt
do
    gzip -k -c "$name" > "$archives/$(basename "$name" .txt).gz"
done
for name in "$path"/*.sh
do
    zip "$archives/$(basename "$name" .sh).zip" $name
done 
