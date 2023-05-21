#!/bin/bash

if [ $# -ge 1 ]
then
    path=$1
    shift
else
    path=~
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
        elif [ -f "$name" ] && ! [ -L "$name" ]
        then
            extension=$(rev <<< "$name" | cut -d '.' -f 1 | rev)
            if [ "$extension" == "jpg" -o "$extension" == "gif" -o "$extension" == "png" ] && ! [[ "$name" =~ ^$HOME/html/muzica/.*$ ]]
            then
                newname=~/html/imagini/"$(basename "$name" ".$extension")"
                if [ -e "$newname.$extension" ]
                then
                    if ! [ "$name" -ef "$newname.$extension" ]
                    then
                        number=1
                        while [ -e "${newname}_$number.$extension" ] && ! [ "$name" -ef "${newname}_$number.$extension" ]
                        do
                            let ++number
                        done
                        newname="${newname}_$number"
                        if ! [ "$name" -ef "$newname.$extension" ]
                        then
                            ln "$name" "$newname.$extension"
                        fi
                    fi
                else
                    ln "$name" "$newname.$extension"
                fi
                images[$n]=$newname
                extensions[$n]=$extension
                let ++n
            fi
        fi
    done
}

sort() {
    i=0
    while [ $i -lt $(( $n - 1 )) ]
    do
        j=$i
        while [ $j -lt $n ]
        do
            if [ $(stat -c %Y "$images[$i].$extensions[$i]") -gt $(stat -c %Y "$images[$j].$extensions[$j]") ]
            then
                aimage="${images[$i]}"
                aextension="${extensions[$i]}"
                images[$i]="${images[$j]}"
                extensions[$i]="${extensions[$j]}"
                images[$j]="aimage"
                extensions[$j]="aextension"

            fi
            let ++j
        done
        let ++i
    done
}

make_gallery() {
    i=0
    while [ $i -lt $n ]
    do
        echo "$(basename "${images[$i]}"): <a href=\"${images[$i]}.${extensions[$i]}\">$(basename "${images[$i]}")</a>" >> "$galerie"
        echo "<br>" >> "$galerie"
        let ++i
    done
}

chmod a+x ~

if ! [ -d ~/html ]
then
    mkdir ~/html
fi

chmod a+x ~/html

if ! [ -d ~/html/imagini ]
then
    mkdir ~/html/imagini
fi

if [ -n "$(ls -A ~/html/imagini)" ]
then
    for name in ~/html/imagini/*
    do
        rm "$name"
    done
fi

chmod a+x ~/html/imagini

galerie=~/html/galerie.html
truncate -s 0 "$galerie"

chmod a+r "$galerie"

declare -a images
declare -a extensions

n=0

rec "$path"

sort

make_gallery
