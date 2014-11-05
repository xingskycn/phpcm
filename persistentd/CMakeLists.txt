cmake_minimum_required (VERSION 2.8.9)
project (persistentd)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX__FLAGS} -O2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")

include_directories("include/" "./")

configure_file(config.h.in config.h)

add_executable(persistentd persistentd.cpp)
target_link_libraries(persistentd reactor)
target_link_libraries(persistentd debugso)

install (TARGETS persistentd DESTINATION /usr/bin/)

