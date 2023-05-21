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
    d=0
    fifo=0
    for name in "$1"/*
    do
        if [ -d "$name" ]
        then
            d=$(bc <<< "$d + 1")
            if [ -r "$name" -a -x "$name" ] && ! [ -L "$name" ]
            then
                i=$(rec "$name")
                dd=$(tail -n 1 <<< $i | rev | cut -d ' ' -f 2 | rev)
                dfifo=$(tail -n 1 <<< $i | rev | cut -d ' ' -f 1 | rev)

                d=$(bc <<< "$d + $dd")
                fifo=$(bc <<< "$fifo + $dfifo")

                echo "$i"
            fi
        elif [ -p "$name" ]
        then
            fifo=$(bc <<< "$fifo + 1")
        fi
    done
    echo "$1: $d $fifo"
}

rec "$path"
