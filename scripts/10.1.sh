#!/bin/sh
sleep 1
echo $(od -vAn -N1 -tu1 < /dev/random) > c1