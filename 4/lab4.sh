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

count=0
result1=0
while [ $count -lt $m ]
do
    result1=$(bc -l <<< $result1+$n)
    let ++count
done

count=0
result2=1
while [ $count -lt $m ]
do
    result2=$(bc -l <<< $result2*$n)
    let ++count
done

echo "n * m (or $n * $m) is $result1"
echo "n ^ m (or $n ^ $m) is $result2"

exit 0