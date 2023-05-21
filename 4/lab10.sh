#!/bin/bash

if [ $# -gt 2 ]
then
    echo "Too many arguments have been passed: '$@'"
    exit 1
elif [ $# -eq 2 ]
then
    n=$1
    m=$2
elif [ $# -eq 1 ]
then
    n=$1
fi

while ! [[ $n =~ ^[1-9][0-9]*$ ]]
do
    read -p "Insert n: " n
done
while ! [[ $m =~ ^[1-9][0-9]*$ ]]
do
    read -p "Insert m: " m
done

isPowerOf() {
    local -n result=$3
    if [ $1 -eq 0 ]
    then
        result=false
    elif [ $1 -eq 1 ]
    then
        result=true
    else
        if [ $(( $1 % $2 )) -ne 0 ]
        then
            result=false
        else
            isPowerOf $(( $1 / $2 )) $2 $3
        fi
    fi
}

isPowerOf $n $m a
echo $a