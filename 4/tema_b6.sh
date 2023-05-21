#!/bin/bash

solution() {
    local -n power=$2
    local -n result=$3
    if [ $1 == "0" ]
    then
        result=$(bc <<< "0")
        power=$(bc <<< "1")
    else
        solution $(bc <<< "$1 / 10") $2 $3
        result=$(bc <<< "$result + (9 - $1 % 10) * $power")
        power=$(bc <<< "$power * 10")
    fi
}

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

solution $n p a
echo $a