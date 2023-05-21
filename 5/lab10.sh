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
set -x
rec() {
    echo "$2$(basename "$1")"
    for name in "$1"/*
    do
        if [ -d "$name" -a -r "$name" -a -x "$name" ] && ! [ -L "$name" ]
        then
            rec "$name" "$2    "
        elif [ -f "$name" -a -r "$name" ] && ! [ -L "$name" ]
        then
            echo "$2    $(basename "$name")"
        elif [ -f "$name" -o -d "$name" ]
        then
            echo "$2    $(basename"$name")----"
        fi
    done
}

rec "$path"
