#include <queue>

#include "reactor.hpp"

#include "IServerThread.hpp"

#define BackendResponder ReactorUserCallback
#define BackendResponderNamed ReactorUserCallbackNamed

class Backend : public IServerThread {
    public:
        Backend();
        void setCallback(BackendResponder);
        virtual void* start(); //start server
        void addEvent(ReactorEStruct event);
    private:
        Reactor *reactor;
        std::queue<ReactorEStruct> inQueue;
        BackendResponderNamed;
};
