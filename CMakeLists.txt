cmake_minimum_required(VERSION 3.10)

project(experimental)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)

add_executable(experimental src/main.cpp)