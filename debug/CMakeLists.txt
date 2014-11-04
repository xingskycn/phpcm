cmake_minimum_required (VERSION 2.8.9)
project (debug)

#set(CMAKE_COMPILER_IS_GNUCXX true)
#set(CMAKE_CXX_COMPILER "/usr/bin/g++-4.9")
#set(CMAKE_C_COMPILER "/usr/bin/gcc-4.9")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX__FLAGS} -std=c++11 -O3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -O3")
set(CMAKE_EXE_LINKER_FLAGS, "${CMAKE_EXE_LINKER_FLAGS} -pthread ")

#set(RELEASE ON CACHE BOOL "Rel")
set(RELEASE OFF CACHE BOOL "Rel")

if (RELEASE)
    set(CMAKE_BUILD_TYPE, "Release")
    set(DEBUG_CPP "debug.release.cpp")
    set(_RELEASE_ ON CACHE BOOL "Release")
else (RELEASE)
    set(CMAKE_BUILD_TYPE, "Debug")
    set(DEBUG_CPP "debug.debug.cpp")
    set(_RELEASE_ OFF)
endif (RELEASE)

configure_file(config.h.in config.h)

add_library (debuga ${DEBUG_CPP})
add_library (debugso SHARED ${DEBUG_CPP})
install (TARGETS debugso DESTINATION /usr/lib/)
