cmake_minimum_required(VERSION 3.28)
project(techniki_inteligencji_obliczeniowej)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -static-libgcc -static-libstdc++")

add_executable(knapsack
    src/knapsack.cpp
    src/helper.cpp
    src/rng.cpp
    src/io_utils.cpp
)

target_include_directories(knapsack
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
