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
        if [ $1 -le 1 ]
        then
            cache[$1]=1
        else
            x $(( $1 - 1 )) > /dev/null
            x $(( $1 - 2 )) > /dev/null
            cache[$1]=$(bc <<< "($1 - 1) * ${cache[$(( $1 - 1 ))]} + ($1 - 2) * ${cache[$(( $1 - 2 ))]}")
        fi
    fi
    echo ${cache[$1]}
}
x $n