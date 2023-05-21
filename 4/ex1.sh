#!/bin/bash

if [ $# -ge 1 ]
then
  p=$1
  shift
fi

function prim()
{
  if [ $1 -le 1 ]
  then
    return 0 
  else
    for i in $(seq 2 1 $(($1/2)))
    do
      if [ $(($1 % $i)) -eq 0 ]
      then
      return 0
      fi
    done
    return 1
  fi
}

while ! ( [[ $p =~ ^[1-9][0-9]*$ ]] && ! $(prim $p) )
do
  read -p "Introduceti un numar valid " p
done

if [ $# -ge 1 ]
then
  n=$1
  shift
fi

while ! [[ $n =~ ^[1-9][0-9]*$ ]]
do
  read -p "Introduceti un numar valid " n
done

declare -a v
for i in $(seq 1 1 $#)
do
  v[$(($i-3))]=${!i}
done
for i in $(seq $(($#+1)) 1 $(($n)))
do
  read -p "Introduceti numarul $i " v[i]
done
declare -i sum

sum=0
for i in ${v[@]}     #$(seq 3 1 $(($2+2)))
do

  if [ $(($i % $p)) -eq 0 ]
  then
    sum=$(($sum + $i*$i*$i))
  fi
done

echo $sum
# set -x pt debug