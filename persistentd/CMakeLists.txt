cmake_minimum_required (VERSION 2.8.9)
project (persistentd)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX__FLAGS} -O2 -pthread -DWITH_HASHSET=1")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")

include_directories("include/" "./")

configure_file(config.h.in config.h)

add_executable(persistentd persistentd.cpp frontend.cpp thread.cpp backend.cpp storage.cpp)
target_link_libraries(persistentd reactor)
target_link_libraries(persistentd debugso)

add_executable(test test.cpp frontend.cpp thread.cpp backend.cpp storage.cpp)
target_link_libraries(test reactor)
target_link_libraries(test debugso)


install (TARGETS persistentd DESTINATION /usr/bin/)

