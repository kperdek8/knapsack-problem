cmake_minimum_required(VERSION 3.25)
project(techniki_inteligencji_obliczeniowej)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -static-libgcc -static-libstdc++")

add_executable(knapsack
        src/knapsack_genetic.cpp
               src/helper.cpp
               src/rng.cpp
               src/io_utils.cpp
               src/pop_init.cpp
               src/pop_fit.cpp
               src/pop_crossover.cpp
               src/pop_mutation.cpp
               src/pop_selection.cpp
               src/parser.cpp
)

target_include_directories(knapsack
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

add_executable(knapsack_ant
        src/knapsack_ant.cpp
        src/helper.cpp
        src/rng.cpp
        src/io_utils.cpp
        src/pop_init.cpp
        src/pop_fit.cpp
        src/parser.cpp
)

target_include_directories(knapsack_ant
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
