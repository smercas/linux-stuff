#!/bin/bash

isPrime() {
    if [ $1 -lt 2 ]
    then
        echo false
        return 0
    elif [ $(( $1 % 2 )) -eq 0 -a $1 -ne 2 ]
    then
        echo false
        return 0
    else
        i=3
        while [ $(( $i ** 2 )) -le $1 ]
        do
            if [ $(( $1 % $i )) -eq 0 ]
            then
                echo false
            return 0
            fi
            let i=$i+2
        done
    fi
    echo true
    return 0
}

if [ $# -ge 1 ]
then
    p=$1
    shift
fi

while ! [[ $p =~ ^[1-9][0-9]*$ && $(isPrime $p) == true ]]
do
    read -p "Insert p: " p
done


if [ $# -ge 1 ]
then
    n=$1
    shift
fi

while ! [[ $n =~ ^[1-9][0-9]*$ ]]
do
    read -p "Insert n: " n
done


if [ $# -eq $n ]
then
    numbers=$@
fi

while ! [[ "$numbers" =~ ^([-]?[1-9][0-9]*|0)([ ]([-]?[1-9][0-9]*|0)){$(( $n - 1 ))}$ ]]
do
    read -p "Insert n numbers: " numbers
done

sum=0
for n in $numbers
do
    if [ $(( $n % $p )) -eq 0 ]
    then
        sum=$(bc <<< "$sum + $n ^ 3")
    fi
done

echo $sum