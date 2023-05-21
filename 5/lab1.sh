#!/bin/bash

if [ $# -ge 1 ]
then
    path=$1
    shift
fi

while ! [ -e "$path" -a -f "$path" -a -x "$path" ]
# x - so the recieved bash script or whatever can be executed
do
    read -p "Given file ($path) either does not exist, is not a file or can't be executed. Insert a new file here: " path
done

if [ $# -ge 1 ]
then
    n=$1
    shift
fi

while ! [[ $n =~ ^[1-9][0-9]*$ ]]
do
    read -p "n ($n) must be a positive integer that's greater than 0. Insert a new n here: " n
done

declare -a numbers

i=0

while [ $i -lt $n ]
do
    if [ $# -ge 1 ]
    then
        numbers[$i]=$1
        shift
    fi
    while ! [[ ${numbers[$i]} =~ ^([-]?[1-9][0-9]*|0)$ ]]
    do
        read -p "v[$i] (${numbers[$i]}) either has not been passed or is not a valid integer. Insert a new v[$i] here: " numbers[$i]
    done
    let ++i
done

i=0

while [ $i -lt $n ]
do
    "$path" ${numbers[$i]} &
    let ++i
done

echo start

wait

echo end
