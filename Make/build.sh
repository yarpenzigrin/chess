#!/usr/bin/bash

mkdir -p build
g++ -std=c++14 -I src test/main.cpp -o build/tests && build/tests