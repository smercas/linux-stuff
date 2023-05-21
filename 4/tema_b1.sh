#!/bin/bash

if [ $# -gt 2 ]
then
    echo "Too many arguments have been passed: '$@'"
    exit 1
elif [ $# -eq 2 ]
then
    k=$1
    d=$2
elif [ $# -eq 1 ]
then
    k=$1
fi

while ! [[ $k =~ ^[1-9][0-9]*$ ]]
do
    read -p "Insert k: " k
done
while ! [[ $d =~ ^[1-9][0-9]*$ ]]
do
    read -p "Insert d: " d
done

sum=0
n=1
while [ $n -le $k ]
do
    sum=$(bc <<< "$sum + ($n * $d) ^ 2")
    let ++n
done

echo $sum