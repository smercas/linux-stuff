#!/bin/bash

if [ $# -eq 0 ]
then
    echo "no arguments have been passed, why do you need to ponder the average of nothing?"
    exit 1
else
    numbers=$@
fi

while [[ $numbers =~ ^([-]?[1-9][0-9]*|0)([ ]([-]?[1-9][0-9]*|0))*$ ]]
do
    read -p "pls gimme some numbers: " numbers
done

result=0
count=0
for n in $numbers
do
    result=$(bc -l <<< "$result + $n")
    let ++count
done

result=$(bc -l <<< "$result / $count")

echo $result