#!/bin/bash

if [ $# -ge 1 ]
then
    uid=$1
    shift
fi

while ! [[ $uid =~ ^([1-9][0-9]*|0)$ ]]
do
    read -p "Invalid user ID recieved ($uid). Send another one: " uid
done

info=$(grep -P "^[^:]*:[^:]*:$uid:" /etc/passwd)

number=$(grep -P "^[^:]*:[^:]*:$uid:" /etc/passwd | wc -l)


if [ $number -gt 1 ]
then
    echo "somehow two or more users have the same UID. This deserves a forced termination."
    exit 1
elif [ $number -eq 0 ]
then
    echo "no user with the UID of $uid has been found."
    exit 1
fi

username=$(grep -P "^[^:]*:[^:]*:$uid:" /etc/passwd | cut -d : -f 1)

fullname=$(grep -P "^[^:]*:[^:]*:$uid:" /etc/passwd | cut -d : -f 5 | cut -d , -f 1)

groups=$(grep -P "^[^:]*:[^:]*:[^:]*:(.*,)?$username(,.*)?$" /etc/group | cut -d : -f 1)

echo "Username: $username"

echo "Name: $fullname"

echo "Groups: $groups"
