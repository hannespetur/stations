cmake_minimum_required (VERSION 2.8.11)
project (stations CXX)

add_executable(count_if_station count_if_station.cpp)
target_link_libraries (count_if_station ${CMAKE_THREAD_LIBS_INIT})

add_executable(count_if_gnu count_if_gnu.cpp)
target_link_libraries (count_if_gnu ${CMAKE_THREAD_LIBS_INIT})
