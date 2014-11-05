#include <map>
#include <string>

#include "IServerThread.hpp"

#define FrontendCScocket int

typedef struct {
    char* data;
    size_t length;
} FrontendBVal;

class Frontend : public IServerThread {
    public:
        virtual void* start(); //start server
    private:
        
        Frontend& operator<<(FrontendBVal add); //in
        
        std::map<FrontendCScocket, FrontendBVal> buffers;
};
