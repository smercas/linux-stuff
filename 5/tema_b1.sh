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
            if [[ "$name" =~ ^.*\.mp3$ ]] && ! [[ "$name" =~ ^$HOME/html/muzica/.*$ ]]
            then
                newname=~/html/muzica/"$(basename "$name" ".mp3")"
                if [ -e "$newname.mp3" ]
                then
                    if ! [ "$name" -ef "$newname.mp3" ]
                    then
                        number=1
                        while [ -e "${newname}_$number.mp3" ] && ! [ "$name" -ef "${newname}_$number.mp3" ]
                        do
                            let ++number
                        done
                        newname="${newname}_$number"
                        if ! [ "$name" -ef "$newname.mp3" ]
                        then
                            ln "$name" "$newname.mp3"
                        fi
                    fi
                else
                    ln "$name" "$newname.mp3"
                fi
                echo "$(basename "$newname"): <a href=\"$newname.mp3\">$(basename "$newname")</a>" >> "$playlist"
                echo "<br>" >> "$playlist"
            fi
        fi
    done
}

chmod a+x ~

if ! [ -d ~/html ]
then
    mkdir ~/html
fi

chmod a+x ~/html

if ! [ -d ~/html/muzica ]
then
    mkdir ~/html/muzica
fi

if [ -n "$(ls -A ~/html/muzica)" ]
then
    for name in ~/html/muzica/*
    do
        rm "$name"
    done
fi

chmod a+x ~/html/muzica

playlist=~/html/playlist.html
truncate -s 0 "$playlist"

chmod a+r "$playlist"

rec "$path"
