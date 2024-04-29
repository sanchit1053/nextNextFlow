#!/bin/sh
sleep 1
val=`cat c1`
if [ "${#val}" -gt 5 ]; then
  echo $val > c2
else
  echo $val > c3
fi