#!/bin/sh
sleep 1
val=`cat c4`
inc=`cat disk`
if [ -z "${inc}" ]; then
    inc=$val
    echo $inc > disk
fi
ans=$((val + inc))

echo $ans > c5