cmake_minimum_required (VERSION 2.8.11)
project (stations CXX)

find_package (Threads)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -D_GLIBCXX_PARALLEL -O3 -std=c++11 -lrt -W -Wall -Wno-long-long -pedantic -Wno-variadic-macros -fmax-errors=1")

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/catch/single_include) # Include "Catch" library

add_subdirectory(examples)
add_subdirectory(benchmarks)

enable_testing(true)
add_subdirectory(test)
