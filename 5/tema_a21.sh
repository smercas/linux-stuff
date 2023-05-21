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
        # same here(can't skip this check since if the path input from the script can't be accesed/its files can't be read it'll request a new path)
        then
            rec "$name"
        elif [ -f "$name" ] && [ "$(file -b "$name" | cut -d ',' -f 1)" == "Bourne-Again shell script" ]
        then
            #stat "$name"
            #echo ''
            chmod a+x "$name"
            #stat "$name"
            #echo ''
            #echo ''
        fi
    done
}

rec "$path"
