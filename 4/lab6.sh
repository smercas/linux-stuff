#!/bin/bash

solution() {
    min=$1
    max=$1
    shift
    for n in $@
    do
        if [ $n -lt $min ]
        then
            min=$n
        fi
        if [ $n -gt $max ]
        then
            max=$n
        fi
    done
    echo "$min <-> $max"
}

if [ $# -ne 0 ]
then
    numbers=$@
fi

while ! [[ "$numbers" =~ ^([-]?[1-9][0-9]*|0)([ ]([-]?[1-9][0-9]*|0))*$ ]]
do
    read -p "pls gimme some numbers: " numbers
done

solution $numbers