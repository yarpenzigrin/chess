#!/usr/bin/bash

g++ -std=c++14 -ggdb3 -I src test/core.cpp -o build/core_tests
g++ -std=c++14 -ggdb3 -I src test/gameplay.cpp -o build/gameplay_tests
g++ -std=c++14 -ggdb3 -I src test/misc.cpp -o build/misc_tests
g++ -std=c++14 -ggdb3 -I src src/random_game.cpp -o build/random_game -O3