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

if [ $# -ge 1 ]
then
    k=$1
    shift
fi

while ! [[ $k =~ ^[1-9][0-9]*$ ]]
do
    read -p "k ($k) must be a positive integer. Insert a new k here: " k
done

rec() {
    for name in "$1"/*
    do
        if [ -d "$name" -a -r "$name" -a -x "$name" ] && ! [ -L "$name" ]
        # same here(can't skip this check since if the path input from the script can't be accesed/its files can't be read it'll request a new path)
        then
            rec "$name"
        elif [ -f "$name" -a -r "$name" -a -x "$name" ]
        # r - to read the file
        then
            echo "File: $name"
            echo "$(basename "$name"): start"
            head -n $k "$name"
            echo "$(basename "$name"): end"
            echo ""
        fi
    done
}

rec "$path"
