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
//    setlogmask (LOG_UPTO (LOG_NOTICE || LOG_INFO || LOG_WARNING));
    closelog();
}

void startLogging()
{
    openlog("PHPCM", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL6);
}
