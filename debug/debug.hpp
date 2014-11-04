#include "config.h"

void debug_write_log_std(std::string mode, unsigned short breakMode, const char* x, const char* y);

#ifndef _RELEASE_
#include "debug.debug.hpp"
#else
#include "debug.release.hpp"
#endif
