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

rec() {
    for name in "$1"/*
    do
        if [ -d "$name" -a -r "$name" -a -x "$name" ] && ! [ -L "$name" ]
        then
            rec "$name"
        elif [ -f "$name" -a -r "$name" ] && [[ "$name" =~ ^.*\.sh$ ]]
        # r - to read the file
        then
            echo "File: $name"
            echo "$(basename "$name"): start"
            grep '#' "$name" | grep -v '$#' | grep -v '#!' | grep -v -P "\$\{[^}]*#{1,2}[^}]*\}"
            echo "$(basename "$name"): end"
            echo ""
        fi
    done
}

rec "$path"
