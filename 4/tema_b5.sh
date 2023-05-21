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

solution() {
    local -n result=$2
    if [ $1 == "0" ]
    then
        result=$(bc <<< "0")
    else
        solution $(bc <<< "$1 / 10") $2
        if [ $(isPrime $(bc <<< "$1 % 10")) == true ]
        then
            result=$(bc <<< "$result + $1 % 10")
        fi
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

solution $n a
echo $a