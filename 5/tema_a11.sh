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
        elif [ -f "$name" -a -r "$name" -a -x "$name" ] && [[ $name =~ ^.*\.sh$ ]]
        # r - to read the file
        then
            i=$(grep -P "^(.* )?for .*$" "$name")
            if [ -z $i ]
            then
                echo "$name has no for loops"
            else
                echo "The file $name has $(wc -l <<< $i) for loops"
                echo "$(basename "$name"): start"
                echo "$i"
                echo "$(basename "$name"): end"
            fi
            echo ""
        fi
    done
}

rec "$path" 2> "$(dirname "$0")/erori_sintactice.txt"

# ok but what about commented for loops?
# ...
# fuck no I'm not doing none of that
