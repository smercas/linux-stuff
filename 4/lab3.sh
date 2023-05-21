#!/bin/bash

compile()
{
    output=${1%.c}           # output=$(dirname $1)/$(basename $1 .c)    
    gcc "$1" -o "$output" -Wall
}

print()
{
    echo "The file $1 contains:"
    cat "$1"
}

if [ $# -eq 0 ]
then
    echo "How to use this script: $0 <directory>"
    exit 1
fi

if [ ! -d "$1" -o ! -r "$1"  ]
then
    echo "Error: Either you don't have reading rights over $1, or it's not a directory!"
    exit 2
fi

ls -A -1 "$1" | while IFS='' read -r f
do
    if [[ "$f" = *.c ]]
    then
        echo "C source file: $1/$f"
        compile "$1/$f"
    elif [[ "$f" = *.txt ]]
    then
        echo "Text file: $1/$f"
        print "$1/$f"
    else
        echo "File $1/$f not supported"
    fi
done