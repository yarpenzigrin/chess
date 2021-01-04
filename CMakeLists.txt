
cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(chess)

add_library(chess INTERFACE)
target_include_directories(chess INTERFACE include)
target_compile_options(chess INTERFACE
    -O3
    -ggdb3
)

add_executable(example_game examples/random_game.cpp)
target_link_libraries(example_game chess)

add_library(chesstest INTERFACE)
target_include_directories(chesstest INTERFACE test)

add_executable(core_tests test/core.cpp)
target_link_libraries(core_tests chess chesstest)

add_executable(gameplay_tests test/gameplay.cpp)
target_link_libraries(gameplay_tests chess chesstest)

add_executable(misc_tests test/misc.cpp)
target_link_libraries(misc_tests chess chesstest)

add_custom_target(game
    DEPENDS example_game
    COMMAND ./example_game
)

add_custom_target(tests
    DEPENDS core_tests gameplay_tests misc_tests
    COMMAND ./core_tests ; ./gameplay_tests ; ./misc_tests
)