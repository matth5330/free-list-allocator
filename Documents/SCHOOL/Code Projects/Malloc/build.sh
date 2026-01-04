#!/bin/bash
# Build script for the custom allocator

echo "Building custom memory allocator..."
g++ -std=c++11 -Wall -Wextra -O2 -o allocator allocator.cpp main.cpp

if [ $? -eq 0 ]; then
    echo "Build successful! Run with: ./allocator"
else
    echo "Build failed!"
    exit 1
fi


