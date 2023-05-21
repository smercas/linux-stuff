#!/bin/bash

solution() {
    max=0
    min=0
    for n in $@
    do
        if [ $n -lt $min -o $min -eq 0 ]
        then
            min=$n
        fi
        if [ $n -gt $max -o $max -eq 0 ]
        then
            max=$n
        fi
    done
    declare -a f
    i=$min
    while [ $i -le $max ]
    do
        let f[$(($i-$min))]=0
        let ++i
    done
    for n in $@
    do
        let ++f[$(($n-$min))]
    done
    mcount=${f[0]}
    mnumber=$min
    i=$min
    while [ $i -le $max ]
    do
        if [ ${f[$(( $i - $min ))]} -ge $mcount ]
        then
            mnumber=$i
            mcount=${f[$(($i-$min))]}
        fi
        let ++i
    done
    echo $mnumber
}

if [ $# -ne 0 ]
then
    numbers=$@
fi

while ! [[ "$numbers" =~ ^([-]?[1-9][0-9]*|0)([ ]([-]?[1-9][0-9]*|0))*$ ]]
do
    read -p "pls gimme some numbers: " numbers
done

solution $numbers