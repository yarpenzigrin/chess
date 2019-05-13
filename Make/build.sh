#!/usr/bin/bash

mkdir -p build
# g++ -std=c++14 -I src test/core.cpp -o build/core_tests && build/core_tests
g++ -std=c++14 -I src test/gameplay.cpp -o build/gameplay_tests && build/gameplay_tests