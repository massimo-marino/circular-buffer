#!/bin/bash
# 
# File:   oclint.bash
# Author: massimo
#
# Created on May 10, 2017, 3:37:22 PM
#
cp ../cmake-build-debug-cmake-clang-500/compile_commands.json . # ../build/Release/compile_commands.json .
#
oclint -enable-global-analysis \
       -p ../build/Debug/ \
       ./example/circular-buffer-example.cpp circularBuffer.cpp \
       -- -std=gnu++17 -O3 -pthread -Wall -Weffc++ -Wextra -L/usr/lib/x86_64-linux-gnu -I/usr/include -lm
#
rm ./compile_commands.json
#
