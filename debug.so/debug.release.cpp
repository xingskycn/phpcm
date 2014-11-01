#include <string>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <chrono>
#include <map>

#include "debug.hpp"

void (*debug_write_log)(std::string, unsigned short, const char*, const char*);
FILE *LogFile;
unsigned short inFile;

static std::map<pthread_t,long>prevCounts;

void debug_write_log_std(std::string mode, unsigned short breakMode, const char* x, const char* y)
{
    char *outbuffer = new char [strlen(x) + strlen(y) + 1];
    sprintf(outbuffer, x, y);
    if ((breakMode == 0) && (mode.compare(__WARNING_TEXT) != 0 )) {
	printf("(%d:%ld)	[%s]	%s\n", getpid(), pthread_self(), mode.c_str(), outbuffer);
    } else {
	fprintf(stderr, "(%d:%ld)	[%s]	%s\n", getpid(), pthread_self(), mode.c_str(), outbuffer);
    }

    delete outbuffer;
}

void debug_write_log_file(std::string mode, unsigned short breakMode, const char* x, const char* y)
{
    char *outbuffer = new char [strlen(x) + strlen(y) + 1];
    sprintf(outbuffer, x, y);

    if (breakMode == 0) {
	fprintf(LogFile, "(%d:%ld)	[%s]	%s\n", getpid(), pthread_self(), mode.c_str(), outbuffer);
    } else {
	fprintf(LogFile, "(%d:%ld)	[%s]	%s\n", getpid(), pthread_self(), mode.c_str(), outbuffer);
    }
    delete outbuffer;
}

void setLogConsole()
{
    debug_write_log = &debug_write_log_std;
    inFile = 0;
}

void setLogFile(std::string lLogFile)
{
    LogFile = fopen(lLogFile.c_str(), "a+");
    debug_write_log = &debug_write_log_file;
    inFile = 1;
}

void unsetLogs()
{
    if (inFile == 1) {
	fclose(LogFile);
    }
}

void debug_call_log(std::string mode, unsigned short breakMode, const char* x, const char* y)
{
    (*debug_write_log)(mode, breakMode, x, y);
    if (breakMode == DEBUG_EXIT) {
	std::terminate();
    } else if (breakMode == DEBUG_EXCEPTION) {
	throw DEBUG_EXCEPTION;
    }
}
