#!/bin/bash
CC='gcc'
CC='i686-w64-mingw32-gcc-4.9.2.exe'
CC='x86_64-w64-mingw32-gcc-4.9.2.exe'
CC='x86_64-pc-cygwin-gcc-4.9.2.exe'

rm *.o > /dev/null 2>&1

for f in $(ls *.c); do
    echo "Compiling $f"
    ${CC} -c -g $f
    if [ "$?" -ne "0" ]; then
        exit 1
    fi
done

${CC} *.o -g -o nc
#rm *.o > /dev/null 2>&1
