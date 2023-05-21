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

fibo() {
    local -n result=$2
    if [ -z $result ]
    then
        result=$(bc <<< "0")
    fi
    if [ $1 -eq 0 ]
    then
        result=$(bc <<< "$result+0")
    elif [ $1 -eq 1 ]
    then
        result=$(bc <<< "$result+1")
    else
        fibo $(( $1 - 1 )) $2
        fibo $(( $1 - 2 )) $2
    fi
}

fibo $n a
echo $a