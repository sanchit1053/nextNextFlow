#!/bin/sh
sleep 1
val=`cat c1`
data=`cat disk`
echo $val
if [ -z "${data}" ]; then
    echo "$val 1" > disk
    echo $val > c2
else
    prefsum=$(echo $data | cut -d ' ' -f 1)
    n=$(echo $data | cut -d ' ' -f 2)
    n=$(( n + 1 ))
    sum=$(( val + prefsum ))
    avg=$(( sum / n ))
    echo "$sum $n" > disk
    echo $avg > c2
fi