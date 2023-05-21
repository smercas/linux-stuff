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

export BC_LINE_LENGTH=10000
result=1
i=1
while [ $i -le $n ]
do
    result=$(bc <<< "$result * $i")
    let ++i
done

echo $result