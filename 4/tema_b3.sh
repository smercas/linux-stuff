#!/bin/bash

solution() {
    s=$1
    shift
    while [ $# -gt $s ]
    do
        sum=0
        i=1
        while [ $i -le $s ]
        do
            sum=$(bc <<< "$sum + ${!i}")
            let ++i
        done
        echo $sum
        echo $sum >> output.txt
        shift $s
    done
    sum=0
    i=1
    while [ $i -le $# ]
    do
        sum=$(bc <<< "$sum + ${!i}")
        let ++i
    done
    echo $sum
    echo $sum >> output.txt
}

if [ $# -ge 1 ]
then
    n=$1
    shift
    numbers=$@
fi

while ! [[ $n =~ ^[1-9][0-9]*$ ]]
do
    read -p "Insert n: " n
done
while ! [[ "$numbers" =~ ^([-]?[1-9][0-9]*|0)([ ]([-]?[1-9][0-9]*|0))*$ ]]
do
    read -p "Insert some numbers: " numbers
done

solution $n $numbers