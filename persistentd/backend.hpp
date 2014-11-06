#include <queue>

#include "reactor.hpp"

#include "IServerThread.hpp"

#define BackendResponder ReactorUserCallback
#define BackendResponderNamed ReactorUserCallbackNamed

#define Event_On_Set 10
#define Event_On_Get 11
#define Event_On_Del 12
#define Event_On_Exec 100

class Backend : public IServerThread {
    public:
        Backend();
        void setCallback(BackendResponder);
        virtual void* start(); //start server
        void addEvent(ReactorEStruct event);

        ReactorRVal handler_On_set(void *data, size_t length, Reactor* reactor);
        ReactorRVal handler_On_Get(void *data, size_t length, Reactor* reactor);
        ReactorRVal handler_On_Del(void *data, size_t length, Reactor* reactor);
        ReactorRVal handler_On_Exec(void *data, size_t length, Reactor* reactor);
    private:
        Reactor *reactor;
        std::queue<ReactorEStruct> inQueue;

        BackendResponderNamed;
};
