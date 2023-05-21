#!/bin/bash

if [ $# -gt 1 ]
then
    echo "Too many arguments have been passed: '$@'"
    exit 1
elif [ $# -eq 1 ]
then
    n=$1
fi

while ! [[ $n =~ ^([1-9][0-9]*|0)$ ]]
do
    read -p "Insert n: " n
done

factorial() {
    local -n result=$2
    if [ $1 -le 1 ]
    then
        result=$(bc <<< "1")
    else
        factorial $(( $1 - 1 )) $2
        result=$(bc <<< "$result*$1")
    fi
}
export BC_LINE_LENGTH=10000
factorial $n a
echo $a