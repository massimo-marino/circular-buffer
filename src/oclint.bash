#!/bin/bash
#
# File: oclint.bash
#
#find . -name '*.cpp' -print0 | xargs -0 clang-check -p ../build
#
clang-check -p ../build/ \
            ./*.cpp \
            ./example/*.cpp \
            ./unitTests/*.cpp \
            -- -std=c++20 -O0 -pthread -Wall -Wextra -pedantic -I. -lm
#
