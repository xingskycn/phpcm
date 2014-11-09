#ifndef BACKEND_HPP
#define BACKEND_HPP

#include <queue>

#include "reactor.hpp"

#include "packet.hpp"
#include "IServerThread.hpp"
#include "storage.hpp"

#define BackendResponder ReactorUserCallback
#define BackendResponderNamed ReactorUserCallbackNamed

#define Event_On_Set 10
#define Event_On_Get 11
#define Event_On_Del 12
#define Event_On_Exec 100

class Backend : public IServerThread {
    public:
        Backend(Storage *storage);
        void setCallback(BackendResponder);
        virtual void* start(); //start server
        void addEvent(ReactorEStruct event);

        ReactorRVal handler_On_Set(Packet *inPacket, Reactor* reactor);
        ReactorRVal handler_On_Get(Packet *inPacket, Reactor* reactor);
        ReactorRVal handler_On_Del(Packet *inPacket, Reactor* reactor);
        ReactorRVal handler_On_Exec(Packet *inPacket,  Reactor* reactor);

        static ReactorRVal s_handler_On_Set(void *data, size_t length, Reactor* reactor);
        static ReactorRVal s_handler_On_Get(void *data, size_t length, Reactor* reactor);
        static ReactorRVal s_handler_On_Del(void *data, size_t length, Reactor* reactor);
        static ReactorRVal s_handler_On_Exec(void *data, size_t length, Reactor* reactor);
    private:
        static void* unpackEventData(void *data, size_t length);
        static void* unpackEventContext(void *data, size_t length);
        void notify();

        Reactor *reactor;
        Storage *storage;
        std::queue<ReactorEStruct> inQueue;
        pthread_mutex_t waiter;

        BackendResponderNamed;
};

#endif
