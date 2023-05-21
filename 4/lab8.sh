#!/bin/bash

if [ $# -gt 2 ]
then
    echo "Too many arguments have been passed: '$@'"
    exit 1
elif [ $# -eq 2 ]
then
    n=$1
    k=$2
elif [ $# -eq 1 ]
then
    n=$1
fi

while ! [[ $n =~ ^([1-9][0-9]*|0)$ ]]
do
    read -p "Insert n: " n
done
while ! [[ $k =~ ^([1-9][0-9]*|0)$ ]]
do
    read -p "Insert k: " k
done

factorial() {
    result=1
    i=1
    while [ $i -le $1 ]
    do
        let result=$result*$i
        let ++i
    done
    echo $result
}

#echo $(( $(factorial $n) / $(( $(factorial $k) * $(factorial $(( $n - $k ))) )) ))

product() {
    local -n result=$3
    result=1
    i=$1
    while [ $i -le $2 ]
    do
        result=$(bc <<< "$result*$i")
        let ++i
    done
}

declare -i a
declare -i b
if [ $k -le $(( $n / 2 )) ]
then
    product $(( $n - $k + 1 )) $n a
    product 1 $k b
else
    product $(( $k + 1 )) $n a
    product 1 $(( $n - $k )) b
fi
echo $(( $a / $b ))