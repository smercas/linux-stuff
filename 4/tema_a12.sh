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

tetranacci() {
    local -n result=$2
    if [ -z $result ]
    then
        result=$(bc <<< "0")
    fi
    if [ $1 -ge 1 -a $1 -le 3 ]
    then
        result=$(bc <<< "$result+0")
    elif [ $1 -eq 4 ]
    then
        result=$(bc <<< "$result+1")
    else
        tetranacci $(( $1 - 4 )) $2
        tetranacci $(( $1 - 3 )) $2
        tetranacci $(( $1 - 2 )) $2
        tetranacci $(( $1 - 1 )) $2
    fi
}
tetranacci $n a
echo $a