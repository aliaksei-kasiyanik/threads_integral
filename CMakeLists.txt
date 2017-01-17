cmake_minimum_required(VERSION 3.6)
project(threads_integral)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(threads_integral ${SOURCE_FILES})