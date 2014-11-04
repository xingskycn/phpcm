#include "config.h"

#include "debug.release.hpp"

#ifndef _FUNC_DEBUG_

#define IFDEBUG_A if (false) {
#define IFDEBUG_NA if (false) {
#define IFDEBUG_ELSE } else {
#define IFDEBUG_E }

#else

#define IFDEBUG_A if (this->debug) {
#define IFDEBUG_NA if (!this->debug) {
#define IFDEBUG_ELSE } else {
#define IFDEBUG_E }

#endif
