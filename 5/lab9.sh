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
    l=0
    c=0
    for name in "$1"/*
    do
        if [ -d "$name" -a -r "$name" -a -x "$name" ] && ! [ -L "$name" ]
        then
            i=$(rec "$name")
            dl=$(tail -n 1 <<< $i | rev | cut -d ' ' -f 2 | rev)
            dc=$(tail -n 1 <<< $i | rev | cut -d ' ' -f 1 | rev)

            l=$(bc <<< "$l + $dl")
            c=$(bc <<< "$c + $dc")

            echo "$i"
        elif [ -f "$name" -a -r "$name" ] && ! [ -L "$name" ]
        then
            fl=$(cat "$name" | wc -l)
            fc=$(cat "$name" | wc -m)

            l=$(bc <<< "$l + $fl")
            c=$(bc <<< "$c + $fc")
            echo "$name: $fl $fc"
        fi
    done
    echo "$1: $l $c"
}

rec "$path"
