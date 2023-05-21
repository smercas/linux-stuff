#!/bin/bash

declare -i count=0
while [ $# -ne 0 ]
do
    (( ++count ))
    echo "param_${count} = $1"
    shift 1
done

exit 0