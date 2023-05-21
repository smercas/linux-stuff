#!/bin/bash

if [ $# -eq 0 ]
then
    read -p "Insert file name here: " file # /home/stefan/Desktop/lab_so/idk.txt
else
    file=$1
fi

if [ ! -f "$file" -o ! -r "$file"  ]
then
    echo "Error: Either you don\'t have reading rights over $file, or it\'s not a directory!"
    exit 1
fi

touch "$file"
declare -i count=0
declare result
operation() {
    read operand1
    read operator
    read operand2
    if [ "$operator" = '+' ]
    then
        result=$(echo $operand1+$operand2 | bc -l)
    elif [ "$operator" = '-' ]
    then
        result=$(echo $operand1-$operand2 | bc -l)
    elif [ "$operator" = '*' ]
    then
        result=$(echo $operand1*$operand2 | bc -l)
    elif [ "$operator" = '/' ]
    then
        let result1=operand1/operand2
        result2=$(echo $operand1/$operand2 | bc -l)
        result="$result1 (integer division) | $result2 (floating point division)"
    elif [ "$operator" = '%' ]
    then
        let result=operand1%operand2
    elif [ "$operator" = '^' ]
    then
        result=$(echo $operand1^$operand2 | bc -l)
    elif [ "$operator" = 'q' ]
    then
        operator='q'
    else
        operator='q'
    fi

    if [ "$operator" != 'q' ]
    then
        let ++count
        echo "$count: $operand1 $operator $operand2 = $result" >> $file
        operation
    else
        echo "number of operations executed: $count" >> $file
    fi
}

operation