#!/bin/bash

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

x() {
    local -n result=$2
    if [ -z $result ]
    then
        result=$(bc <<< "0")
    fi
    if [ $1 -eq 0 ]
    then
        result=$(bc <<< "$result + 1")
    elif [ $1 -eq 1 ]
    then
        result=$(bc <<< "$result + 2")
    else
        x $(( $1 - 1 )) $2

        x $(( $1 - 2 )) $2
        x $(( $1 - 2 )) $2
        x $(( $1 - 2 )) $2
        x $(( $1 - 2 )) $2

        result=$(bc <<< "$result + $1")
    fi
}
x $n a
echo $a