#include <iostream>
#include <string.h>

#include "reactor.hpp"
#include "debug.hpp"

#include "packet.hpp"
#include "backend.hpp"

Backend::Backend()
{
    reactor = new Reactor();
    reactor->addHandler(Event_On_Set, Backend::s_handler_On_Set);
//    reactor->addHandler(Event_On_Del, Backend::s_handler_On_Set);
//    reactor->addHandler(Event_On_Get, Backend::s_handler_On_Set);
//    reactor->addHandler(Event_On_Exec, Backend::s_handler_On_Set);
}

void Backend::setCallback(BackendResponderNamed)
{
    this->callback = callback;
}

void* Backend::start()
{
    INFO("backend: starting: %s", "start...");
    reactor->runtime();
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
}

/* handlers */

ReactorRVal Backend::handler_On_Set(Packet* inPacket, Reactor *reactor)
{
    DEBUG("backend: handler_On_Set: %s", "start");

//    std::cout << "command:" << inPacket->data << std::endl;
    int client = (inPacket->client);
    std::cout << "command(" << client << ")> " << inPacket->data << std::endl;

    Packet *outPacket = new Packet;
    outPacket->frontendContext = inPacket->frontendContext;
    outPacket->client = inPacket->client;
    outPacket->data = "response from handler_On_Set";

    ReactorRVal forReturn = {
        .data = outPacket,
        .length = sizeof(outPacket),
    };

    DEBUG("backend: handler_On_Set: %s", "done");
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
