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
o="$(dirname "$0")/validate.txt"
truncate -s 0 "$o"

rec() {
    sh=0
    c=0
    cpp=0
    for name in "$1"/*
    do
        if [ -d "$name" -a -r "$name" -a -x "$name" ] && ! [ -L "$name" ]
        then
            i=$(rec "$name")

            dsh=$(tail -n 1 <<< $i | rev | cut -d ' ' -f 3 | rev)
            dc=$(tail -n 1 <<< $i | rev | cut -d ' ' -f 2 | rev)
            dcpp=$(tail -n 1 <<< $i | rev | cut -d ' ' -f 1 | rev)

            sh=$(bc <<< "$sh + $dsh")
            c=$(bc <<< "$c + $dc")
            cpp=$(bc <<< "$cpp + $dcpp")

            echo "$i"
        elif [ -f "$name" ] && ! [ -L "$name" ]
        then
            if [[ "$name" =~ ^.*\.sh$ ]]
            then
                sh=$(bc <<< "$sh + 1")
                echo "$name -- $(md5sum "$name")" >> "$o"
                cat "$name" >> "$o"
                echo "" >> "$o"
            elif [[ "$name" =~ ^.*\.c$ ]]
            then
                c=$(bc <<< "$c + 1")
                echo "$name -- $(sha1sum "$name")" >> "$o"
                cat "$name" >> "$o"
                echo "" >> "$o"
            elif [[ "$name" =~ ^.*\.cpp$ ]]
            then
                cpp=$(bc <<< "$cpp + 1")
                echo "$name -- $(sha256sum "$name")" >> "$o"
                cat "$name" >> "$o"
                echo "" >> "$o"
            fi
        fi
    done
    echo "$1: $sh $c $cpp"
}

rec "$path"
