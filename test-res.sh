#!/bin/bash

gcc -Wall -pthread src/restaurant.c -o restaurant

if [ $? -eq 0 ]; then
    echo "Compilation successful."
    ./restaurant 100 10
else
    echo "Compilation failed."
fi
