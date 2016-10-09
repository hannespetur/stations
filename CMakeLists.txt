cmake_minimum_required (VERSION 2.8.11)
project (stations CXX)

find_package (Threads)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++11 -lrt -W -Wall -Wno-long-long -pedantic -Wno-variadic-macros -fmax-errors=1")
add_subdirectory(examples)

enable_testing(true)
add_subdirectory(test)
