cmake_minimum_required (VERSION 2.8.11)
project (cm)

#execute_process(COMMAND /bin/bash -c "/usr/bin/php-config --includes | sed -e 's/-I//g'" OUTPUT_VARIABLE PHP_INCLUDES)
execute_process(COMMAND /bin/bash -c "/usr/bin/php-config --extension-dir | awk 1 ORS=''" OUTPUT_VARIABLE PHP_OUT)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX__FLAGS} -lmemcached")

#set(RELEASE ON CACHE BOOL "Rel")
set(RELEASE OFF CACHE BOOL "Rel")

if (RELEASE)
    set(CMAKE_BUILD_TYPE, "Release")
    set(_FUNC_DEBUG_ ON CACHE BOOL "Release")
else (RELEASE)
    set(CMAKE_BUILD_TYPE, "Debug")
    set(_FUNC_DEBUG_ OFF)
endif (RELEASE)

configure_file(config.h.in config.h)

include_directories(/usr/include/php5 /usr/include/php5 /usr/include/php5/main /usr/include/php5/TSRM /usr/include/php5/Zend /usr/include/php5/ext /usr/include/php5/ext/date/lib)

add_library (cm SHARED cm.cc cm_class.cc cm_adapter.cc crc32.cc)

target_link_libraries(cm PUBLIC memcached)
target_link_libraries(cm PUBLIC stdc++)

install(TARGETS cm DESTINATION ${PHP_OUT})
