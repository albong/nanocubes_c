#!/bin/bash

rm *.o > /dev/null 2>&1

for f in $(ls *.c); do
    echo "Compiling $f"
    gcc -c $f
    if [ "$?" -ne "0" ]; then
        exit 1
    fi
done

gcc *.o -o nc
