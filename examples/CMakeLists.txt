cmake_minimum_required (VERSION 2.8.11)

add_executable(read_ints_and_keep_primes_only read_ints_and_keep_primes_only.cpp)
add_executable(generate_random_ints_and_sort_them generate_random_ints_and_sort_them.cpp)

set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -std=c++11 -lrt -W -Wall -Wno-long-long -pedantic -Wno-variadic-macros -fmax-errors=1")

target_include_directories(read_ints_and_keep_primes_only PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries (read_ints_and_keep_primes_only ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(generate_random_ints_and_sort_them PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries (generate_random_ints_and_sort_them ${CMAKE_THREAD_LIBS_INIT})
