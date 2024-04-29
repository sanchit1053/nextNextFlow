#!/bin/sh
sleep 1
val=`cat c4`
ans=$((val + val))
echo $ans > c5