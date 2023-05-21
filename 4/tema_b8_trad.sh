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
    if [ $1 -le 1 ]
    then
        result=$(bc <<< "$result + 1")
    else
        local -i i=0
        while [ $i -lt $(( $1 - 2 )) ]
        do
            x $(( $1 - 1 )) $2
            x $(( $1 - 2 )) $2
            let ++i
        done
        x $(( $1 - 1 )) $2
    fi
}
x $n a
echo $a