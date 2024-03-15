#!/bin/bash

echo compiling code
gcc -o sorting sorting.c -pthread
echo running code
echo
./sorting
echo
echo program stopped
echo cleaning up code
echo running output to text file
./sorting > sorting_output.txt
rm sorting