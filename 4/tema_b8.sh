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
    if [ $1 -le 1 ]
    then
        echo 1
    else
        echo $(bc <<< "($1 - 1) * $(x $(( $1 - 1 ))) + ($1 - 2) * $(x $(( $1 - 2 )))")
    fi
}
echo $(x $n)