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
    w=0
    l=0
    for name in "$1"/*
    do
        if [ -d "$name" -a -r "$name" -a -x "$name" ] && ! [ -L "$name" ]
        then
            i=$(rec "$name")
            dw=$(tail -n 1 <<< $i | rev | cut -d ' ' -f 2 | rev)
            dl=$(tail -n 1 <<< $i | rev | cut -d ' ' -f 1 | rev)

            w=$(bc <<< "$w + $dw")
            if [ ${dl} -gt ${l} ]
            then
                l=$dl
            fi
            echo "$i"
        elif [ -f "$name" -a -r "$name" ] && ! [ -L "$name" ] && [[ $(file "$name") =~ ^.*ASCII" "text.*$ ]]
        then
            fw=$(cat "$name" | wc -w)
            fl=$(cat "$name" | wc -L)

            w=$(bc <<< "$w + $fw")
            if [ ${fl} -gt ${l} ]
            then
                l=$fl    
            fi

            echo "$name: $fw $fl"
        fi
    done
    echo "$1: $w $l"
}

rec "$path"
