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
    if [ $1 -eq 0 ]
    then
        echo 1
    elif [ $1 -eq 1 ]
    then
        echo 2
    else
        echo $(bc <<< "$(x $(( $1 - 1 ))) + 4 * $(x $(( $1 - 2 ))) + $1")
    fi
}
echo $(x $n)
