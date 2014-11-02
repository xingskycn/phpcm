cmake_minimum_required (VERSION 2.8.11)
project (phpcm)

#set(CMAKE_CXX_COMPILER "/usr/bin/g++-4.9")
#set(CMAKE_C_COMPILER "/usr/bin/gcc-4.9")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX__FLAGS} -std=c++11 -O3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -O3")

add_subdirectory(debug)
add_subdirectory(phpmodule)
