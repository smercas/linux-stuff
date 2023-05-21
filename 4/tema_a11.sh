#!/bin/bash

if [ $# -gt 1 ]
then
    echo "Too many arguments have been passed: '$@'"
    exit 1
elif [ $# -eq 1 ]
then
    k=$1
fi

while ! [[ $k =~ ^[1-9][0-9]*$ ]]
do
    read -p "Insert k: " k
done

sum=$(bc <<< "0")
i=1

while [ $i -le $k ]
do
    sum=$(bc <<< "$sum + $i ^ 3")
    let ++i
done

echo $sum