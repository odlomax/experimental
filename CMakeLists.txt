cmake_minimum_required(VERSION 3.10)

project(experimental)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)
enable_testing()

add_subdirectory(src)
add_subdirectory(tests)