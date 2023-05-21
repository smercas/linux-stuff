#!/bin/bash

if [ $# -gt 1 ]
then
    echo "Too many arguments have been passed: '$@'"
    exit 1
elif [ $# -eq 1 ]
then
    n=$1
fi

while ! [[ $n =~ ^[1-9][0-9]*$ ]]
do
    read -p "Insert n: " n
done

declare -a cache
x() {
    if [ -z "${cache[$1]}" ]
    then
        if [ $1 -eq 0 ]
        then
            cache[$1]=1
        elif [ $1 -eq 1 ]
        then
            cache[$1]=2
        else
            x $(( $1 - 1 )) > /dev/null
            x $(( $1 - 2 )) > /dev/null
            cache[$1]=$(bc <<< "${cache[$(( $1 - 1 ))]} + 4 * ${cache[$(( $1 - 2 ))]} + $1")
        fi
    fi
    echo ${cache[$1]}
}
x $n