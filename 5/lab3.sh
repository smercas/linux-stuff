#!/bin/bash

if [ $# -ge 1 ]
then
    path=$1
    shift
fi

while ! [ -e "$path" -a -d "$path" -a -r "$path" -a -w "$path" -a -x "$path" ]
# r - to read the contents of the recieved folder
# w - so files/folders can be added to/deleted from it
# x - again, visibility(can't read if you can't see, unless you know braille)
do
    read -p "Given directory ($path) either does not exist, is not a directory, its files can't be read/modified or it can't be accessed. Insert a new directory here: " path
done

if [ $# -ge 1 ]
then
    name=$1
    shift
fi

while ! [ -n "$name" ]
do
    read -p "Give me a normal project name please": name
done

project="$(dirname "$path")/$name"

if ! [ -e "$project" -a -d "$project" ]
then
    mkdir "$project"
fi

if ! [ -e "$project/bin" -a -d "$project/bin" ]
then
    mkdir "$project/bin"
fi

if ! [ -e "$project/obj" -a -d "$project/obj" ]
then
    mkdir "$project/obj"
fi

if ! [ -e "$project/src" -a -d "$project/src" ]
then
    mkdir "$project/src"
fi

if ! [ -e "$project/src/$name.c" -a -f "$project/src/$name.c" ]
then
    touch "$project/src/$name.c"
fi

if [ -e "$project/Makefile" ]
then
    rm "$project/Makefile"
fi

cp ~/'Documents/Makefile_c' "$project/Makefile"

code "$project/src/$name.c"

# just remembered i don't need this since makefiles
# just organise ur project folder like new_c_project and you're good to go
# run "make" while in the project folder to compile everything
# run "make clean" to clean ur project
# i'm not listing the rest of the commands, good luck
# run ./bin/main to execute that shii

#might come back to this someday and do some more stuff
