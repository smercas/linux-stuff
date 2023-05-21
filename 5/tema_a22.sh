#!/bin/bash
set -x
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
o="$(dirname "$0")/validate.txt"
truncate -s 0 "$o"

rec() {
    sh=0
    a=0
    d=0
    for name in "$1"/*
    do
        if [ -d "$name" ] && ! [ -L "$name" ]
        then
            d=$(bc <<< "$d + 1")
            if [ -r "$name" -a -x "$name" ] 
            then
                i=$(rec "$name")

                dsh=$(tail -n 1 <<< $i | rev | cut -d ' ' -f 3 | rev)
                da=$(tail -n 1 <<< $i | rev | cut -d ' ' -f 2 | rev)
                dd=$(tail -n 1 <<< $i | rev | cut -d ' ' -f 1 | rev)

                sh=$(bc <<< "$sh + $dsh")
                a=$(bc <<< "$a + $da")
                d=$(bc <<< "$d + $dd")

                echo "$i"
            fi
        elif [ -f "$name" ] && ! [ -L "$name" ]
        then
            if [ "$(file -b "$name" | cut -d ',' -f 1)" == "Bourne-Again shell script" ]
            then
                sh=$(bc <<< "$sh + 1")
            elif [ $(bc <<< "$(stat -c %X "$name") + 30 * 24 * 60 * 60 - $(date +%s)") -ge 0 ]
            then
                a=$(bc <<< "$a + 1")
            fi
        fi
    done
    echo "$1: $sh $a $d"
}

rec "$path"
