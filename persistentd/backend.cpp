#include <iostream>
#include <string.h>

#include "reactor.hpp"
#include "debug.hpp"

#include "packet.hpp"
#include "backend.hpp"

Backend::Backend(Storage *storage)
{
    this->storage = storage;
    reactor = new Reactor();
    reactor->addHandler(Event_On_Set, Backend::s_handler_On_Set);
    reactor->addHandler(Event_On_Del, Backend::s_handler_On_Del);
    reactor->addHandler(Event_On_Get, Backend::s_handler_On_Get);
    reactor->addHandler(Event_On_Exec, Backend::s_handler_On_Exec);
    pthread_mutex_init(&waiter, NULL);
}

void Backend::setCallback(BackendResponderNamed)
{
    this->callback = callback;
}


void* Backend::start()
{
    INFO("backend: starting: %s", "start...");
    while (true) {
        while (reactor->runtime()) {
            //processing been
        }
        //empty
        pthread_mutex_lock(&waiter);
        pthread_mutex_lock(&waiter);
        pthread_mutex_unlock(&waiter);
    }
    INFO("backend: starting: %s", "ok");
}

void Backend::addEvent(ReactorEStruct event)
{
    ReactorEStruct eventWrapper = {
         .event = event.event,
         .data = new char [sizeof(ReactorEStruct) + sizeof(this)],
         .length = sizeof(ReactorEStruct) + sizeof(this),
        };
    memcpy(eventWrapper.data, this, sizeof(this));
    memcpy((void*)((long)eventWrapper.data + (long)sizeof(this)), &event, sizeof(ReactorEStruct));
    reactor->addEventAsync(eventWrapper, this->callback);
    notify();
}

/* handlers */

ReactorRVal Backend::handler_On_Set(Packet* inPacket, Reactor *reactor)
{
    DEBUG("backend: handler_On_Set: %s", "start");

    int client = (inPacket->client);
    size_t spacepos = inPacket->data.find(" ");
    std::string firstArg = inPacket->data.substr(0, spacepos);
    std::string secondArg = inPacket->data.substr(spacepos+1);
    std::cout << "commandSet(" << client << ")> " << firstArg << "\t" << secondArg << std::endl;

//    bool result = storage->set(firstArg, secondArg);
    std::cout << (long)this->storage << std::endl;
//    bool result = this->storage->set("a", "b");
    bool result = true;

    std::cout << "setSuccess" << std::endl;

    Packet *outPacket = new Packet;
    outPacket->frontendContext = inPacket->frontendContext;
    outPacket->client = inPacket->client;
    if (result) {
        outPacket->data = "SET_OK";
    } else {
        outPacket->data = "SET_FAIL";
    }

    ReactorRVal forReturn = {
        .data = outPacket,
        .length = sizeof(outPacket),
    };

    DEBUG("backend: handler_On_Set: %s", "done");
    return forReturn;
}

ReactorRVal Backend::handler_On_Get(Packet* inPacket, Reactor *reactor)
{
    DEBUG("backend: handler_On_Get: %s", "start");

    int client = (inPacket->client);
    std::cout << "commandGet(" << client << ")> " << inPacket->data << std::endl;

    Packet *outPacket = new Packet;
    outPacket->frontendContext = inPacket->frontendContext;
    outPacket->client = inPacket->client;
    outPacket->data = "response from handler_On_Get";

    ReactorRVal forReturn = {
        .data = outPacket,
        .length = sizeof(outPacket),
    };

    DEBUG("backend: handler_On_Get: %s", "done");
    return forReturn;
}


ReactorRVal Backend::handler_On_Del(Packet* inPacket, Reactor *reactor)
{
    DEBUG("backend: handler_On_Del: %s", "start");

    int client = (inPacket->client);
    std::cout << "commandDel(" << client << ")> " << inPacket->data << std::endl;

    Packet *outPacket = new Packet;
    outPacket->frontendContext = inPacket->frontendContext;
    outPacket->client = inPacket->client;
    outPacket->data = "response from handler_On_Del";

    ReactorRVal forReturn = {
        .data = outPacket,
        .length = sizeof(outPacket),
    };

    DEBUG("backend: handler_On_Del: %s", "done");
    return forReturn;
}

ReactorRVal Backend::handler_On_Exec(Packet* inPacket, Reactor *reactor)
{
    DEBUG("backend: handler_On_Exec: %s", "start");

    int client = (inPacket->client);
    std::cout << "commandExec(" << client << ")> " << inPacket->data << std::endl;

    Packet *outPacket = new Packet;
    outPacket->frontendContext = inPacket->frontendContext;
    outPacket->client = inPacket->client;
    outPacket->data = "response from handler_On_Exec";

    ReactorRVal forReturn = {
        .data = outPacket,
        .length = sizeof(outPacket),
    };

    DEBUG("backend: handler_On_Exec: %s", "done");
    return forReturn;
}


/* helpers */

ReactorRVal Backend::s_handler_On_Set(void *data, size_t length, Reactor *reactor)
{
    void *unpacked_data = unpackEventData(data, length);
    void *context = unpackEventContext(data, length);
    ReactorRVal forReturn = ((Backend *)context)->handler_On_Set((Packet*)(((ReactorEStruct*)unpacked_data)->data), reactor);
    delete (char*)data;
    delete (char*)unpacked_data;
    return forReturn;
}

ReactorRVal Backend::s_handler_On_Get(void *data, size_t length, Reactor *reactor)
{
    void *unpacked_data = unpackEventData(data, length);
    void *context = unpackEventContext(data, length);
    ReactorRVal forReturn = ((Backend *)context)->handler_On_Get((Packet*)(((ReactorEStruct*)unpacked_data)->data), reactor);
    delete (char*)data;
    delete (char*)unpacked_data;
    return forReturn;
}

ReactorRVal Backend::s_handler_On_Del(void *data, size_t length, Reactor *reactor)
{
    void *unpacked_data = unpackEventData(data, length);
    void *context = unpackEventContext(data, length);
    ReactorRVal forReturn = ((Backend *)context)->handler_On_Del((Packet*)(((ReactorEStruct*)unpacked_data)->data), reactor);
    delete (char*)data;
    delete (char*)unpacked_data;
    return forReturn;
}

ReactorRVal Backend::s_handler_On_Exec(void *data, size_t length, Reactor *reactor)
{
    void *unpacked_data = unpackEventData(data, length);
    void *context = unpackEventContext(data, length);
    ReactorRVal forReturn = ((Backend *)context)->handler_On_Exec((Packet*)(((ReactorEStruct*)unpacked_data)->data), reactor);
    delete (char*)data;
    delete (char*)unpacked_data;
    return forReturn;
}


/* private */

void* Backend::unpackEventData(void *data, size_t length)
{
    char *bytes = new char [length-sizeof(void*)];
    memcpy(bytes, (void*)((long)data+(long)sizeof(void*)), length-sizeof(void*));
    return bytes;
}

void* Backend::unpackEventContext(void *data, size_t length)
{
    long context_l;
    context_l = *(char*)data;
    void* context = (void*)context_l;
    return context;
}

void Backend::notify()
{
    pthread_mutex_unlock(&waiter);
}
