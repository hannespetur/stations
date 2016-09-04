cmake_minimum_required (VERSION 2.8.11)
project (stations CXX)

find_package (Threads)

add_subdirectory(examples)

enable_testing(true)
add_subdirectory(test)
