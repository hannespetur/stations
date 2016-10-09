cmake_minimum_required (VERSION 2.8.11)
project (stations CXX)

set(stations_test_files
  test.cpp
  test_all_of.cpp
  test_any_of.cpp
  test_count_if.cpp
  test_count.cpp
  test_fill.cpp
  test_for_each.cpp
  test_internal.cpp
  test_none_of.cpp
  test_partition_iterator.cpp
  test_sort.cpp
  test_split.cpp
)

# Include "Catch" library
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/catch/single_include)

add_executable(test_stations ${stations_test_files})

# Link to threading library (pthreads on linux)
target_link_libraries (test_stations ${CMAKE_THREAD_LIBS_INIT})

add_test(NAME CatchTests COMMAND test_stations)
