#include <string>

#ifdef __cplusplus
extern "C" {
#endif
    void debug_call_log(std::string mode, unsigned short breakMode, const char* x, const char* y);
    void setLogConsole();
    void setLogFile();
    void unsetLogs();
#ifdef __cplusplus
}
#endif

#define DEBUG_EXIT 10
#define DEBUG_EXCEPTION 9
#define DEBUG_PASS 0

//stop process full, dump core
#define FATAL(x, y) debug_call_log(" fatal ", DEBUG_EXIT, x, y); std::terminate()

//stop working thread or event
#define ERROR(x, y) debug_call_log(" error ", DEBUG_EXCEPTION, x, y); std::terminate()

#define __WARNING_TEXT "warning"
//non stop working trouble event
#define WARNING(x, y) debug_call_log(__WARNING_TEXT, DEBUG_PASS, x, y)

//infomation log
#define INFO(x, y) debug_call_log(" info  ", DEBUG_PASS, x ,y)

//debug log
//#define DEBUG(x, y) debug_call_log(" debug ", DEBUG_PASS, x, y)
#define DEBUG(x, y) false

//debug log (detail level 2)
//#define DEBUG2(x, y) debug_call_log(" debug ", DEBUG_PASS, x, y)
#define DEBUG2(x, y) false
