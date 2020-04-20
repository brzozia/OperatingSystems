#!/bin/bash

msg=`ipcs -q | awk '{ print $2 }' | tail -n +4`

for i in $msg
do
        ipcrm msg $i
done
