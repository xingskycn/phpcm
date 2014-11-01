#include <string>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <chrono>
#include <map>

#include <ios>
#include <fstream>

#include "debug.hpp"

void (*debug_write_log)(std::string, unsigned short, const char*, const char*);
FILE *LogFile;
unsigned short inFile;

static std::map<pthread_t,long>prevCounts;

void memory_usage(double *vm_usage, double *resident_set)
{
    using std::ios_base;
    using std::ifstream;
    using std::string;

    (*vm_usage) = 0.0;
    (*resident_set) = 0.0;

    ifstream stat_stream("/proc/self/stat", ios_base::in);

    string pid, comm, state, ppid, pgrp, session, tty_nr;
    string tpgid, flags, minflt, cminflt, majflt, cmajflt;
    string utime, stime, cutime, cstime, priority, nice;
    string O, itrealvalue, starttime;

    unsigned long vsize;
    long rss;
    
    stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
		>> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
		>> utime >> stime >> cutime >> cstime >> priority >> nice
		>> O >> itrealvalue >> starttime >> vsize >> rss;

    stat_stream.close();

    long page_size_kb = sysconf(_SC_PAGE_SIZE);
    (*vm_usage) = vsize / 1024.0;
    (*resident_set) = rss * page_size_kb;
}

void debug_write_log_std(std::string mode, unsigned short breakMode, const char* x, const char* y)
{
    char *outbuffer = new char [strlen(x) + strlen(y) + 1];
    sprintf(outbuffer, x, y);
    std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds >(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    );
    long prevCount;
    if (prevCounts.count(pthread_self()) == 0) {
	prevCounts[pthread_self()] = ms.count();
	prevCount = ms.count();
    } else {
	prevCount = prevCounts[pthread_self()];
    }
    double vm;
    double rss;
    memory_usage(&vm, &rss);
    if ((breakMode == 0)&& (mode.compare(__WARNING_TEXT) != 0 )) {
	printf("(%d:%ld)	[%s]	%ld / %ld	rss=%f,	vm=%f	%s\n", getpid(), pthread_self(), mode.c_str(), ms.count(), ms.count()-prevCount, rss, vm, outbuffer);
    } else {
	fprintf(stderr, "(%d:%ld)	[%s]	%ld / %ld	rss=%f,	vm=%f	%s\n", getpid(), pthread_self(), mode.c_str(), ms.count(), ms.count()-prevCount, rss, vm, outbuffer);
    }
    prevCounts[pthread_self()] = ms.count();

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
