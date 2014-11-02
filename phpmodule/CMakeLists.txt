cmake_minimum_required (VERSION 2.8.11)
project (cm)

execute_process(COMMAND /bin/bash -c "/usr/bin/php-config --includes | sed -e 's/-I//g'" OUTPUT_VARIABLE PHP_INCLUDES)
execute_process(COMMAND /usr/bin/php-config --extension-dir OUTPUT_VARIABLE PHP_OUT)

#include_directories(${PHP_INCLUDES})
include_directories(/usr/include/php5 /usr/include/php5 /usr/include/php5/main /usr/include/php5/TSRM /usr/include/php5/Zend /usr/include/php5/ext /usr/include/php5/ext/date/lib)

add_library (cm SHARED cm.cc cm_class.cc cm_adapter.cc crc32.cc)
install(TARGETS cm DESTINATION ${PHP_OUT})
