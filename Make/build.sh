#!/usr/bin/bash

g++ -std=c++14 -ggdb3 -I include test/core.cpp -o build/core_tests
g++ -std=c++14 -ggdb3 -I include test/gameplay.cpp -o build/gameplay_tests
g++ -std=c++14 -ggdb3 -I include test/misc.cpp -o build/misc_tests
g++ -std=c++14 -ggdb3 -I include examples/random_game.cpp -o build/random_game -O3