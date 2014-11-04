#include <string>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <chrono>
#include <map>

#include <syslog.h>

#include "debug.hpp"

static std::map<pthread_t,long>prevCounts;

void debug_write_log_std(std::string mode, unsigned short breakMode, const char* x, const char* y)
{
    char *outbuffer = new char [strlen(x) + strlen(y) + 1];
    sprintf(outbuffer, x, y);
    if ((breakMode == 0) && (mode.compare(__WARNING_TEXT) != 0 )) {
	syslog(6, "(%d:%ld) [%s] %s\n", getpid(), pthread_self(), mode.c_str(), outbuffer);
    } else {
	syslog(6, "(%d:%ld) [%s] %s\n", getpid(), pthread_self(), mode.c_str(), outbuffer);
    }

    delete outbuffer;
}
