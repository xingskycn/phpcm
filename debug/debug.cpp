#include <string>

#include <syslog.h>

#include "debug.hpp"

void debug_call_log(std::string mode, unsigned short breakMode, const char* x, const char* y)
{
    debug_write_log_std(mode, breakMode, x, y);
    if (breakMode == DEBUG_EXIT) {
        std::terminate();
    } else if (breakMode == DEBUG_EXCEPTION) {
        throw DEBUG_EXCEPTION;
    }
}

void stopLogging()
{
    openlog("PHPCM", 0, LOG_LOCAL6);
}

void startLogging()
{
    closelog();
}
