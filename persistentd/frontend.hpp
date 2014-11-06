#ifndef Frontend_HPP
#define Frontend_HPP

#include <map>
#include <string>

#include "IServerThread.hpp"
#include "backend.hpp"

#define FrontendCSocket int

typedef struct {
    std::string data;
    size_t length;
    FrontendCSocket socket;
} FrontendBVal;

class Frontend : public IServerThread {
    public:
        Frontend(Backend* backend);
        virtual void* start(); //start server

        void onResponse(FrontendCSocket csocket, std::string response);

        static void s_onResponse(ReactorRVal ret);

//temporary public
        Frontend& operator<<(FrontendBVal add); //in
    private:
        Backend* backend;
        std::map<FrontendCSocket, std::string> buffers;
};



#endif
