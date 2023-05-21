#!/bin/bash

if [ $# -ge 1 ]
then
    user=$1
    shift
else
    read -p "Insert a user name here or type nothing then press enter to close the script: " user
fi

while [ -n "$user" ]
do
    i=$(id "$user" 2> /dev/null)
    if [ -n "$i" ]
    then
        j=$(last -s -30days "$user" | head -n -2)
        if [ -z "$j" ]
        then
            echo "The user $user has never connected."
            echo ''
        else
            echo "$(head -n 3 <<< "$j")"
        fi
        i=''
        while [ -n "$user" ] && [ -z "$i" ]
        do
            if [ $# -ge 1 ]
            then
                user=$1
                shift
            else
                read -p "Insert another one or leave this space blank and press enter to close the script: " user
            fi
            i=$(id "$user" 2> /dev/null)
            if [ -n "$user" ] && [ -z "$i" ]
            then
                echo "Recieved user name does not exist."
            fi
        done
    else
        echo "Recieved user name does not exist."
        while [ -n "$user" ] && [ -z "$i" ]
        do
            if [ $# -ge 1 ]
            then
                user=$1
                shift
            else
                read -p "Insert another one or leave this space blank and press enter to close the script: " user
            fi
            i=$(id "$user" 2> /dev/null)
            if [ -z "$i" ]
            then
                echo "Recieved user name does not exist."
            fi
        done
    fi
done