#!/bin/sh
sleep 1
val=`cat c2`
echo $val
if [ $val -ge 42 ]; then
  echo $val > c3
else
  echo $val > c4
fi