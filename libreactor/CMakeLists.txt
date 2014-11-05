cmake_minimum_required (VERSION 2.8.9)
project (reactor)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX__FLAGS} -O2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
#set(CMAKE_EXE_LINKER_FLAGS, "${CMAKE_EXE_LINKER_FLAGS} -pthread ")

#configure_file(config.h.in config.h)

#add_library (debuga debug.cpp ${DEBUG_CPP})
#add_library (debugso SHARED debug.cpp ${DEBUG_CPP})
#install (TARGETS debugso DESTINATION /usr/lib/)
add_library(reactor SHARED reactor.cpp)
add_executable(test_reactor reactor.cpp main.cpp)
install (TARGETS reactor DESTINATION /usr/lib/)

