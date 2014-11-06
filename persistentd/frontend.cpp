#include <iostream>

#include "debug.hpp"

#include "frontend.hpp"
#include "packet.hpp"

/* object manage */

Frontend::Frontend(Backend *backend)
{
    this->backend = backend;
    this->backend->setCallback(&Frontend::s_onResponse);
}

/* public interface */

void* Frontend::start()
{
    INFO("frontend: starting: %s", "start...");
//    std::string s("str");
//    FrontendBVal add = { .data = s, .length=sizeof(s), .socket=1 };
//    *this << add;
    INFO("frontend: starting: %s", "ok");
}

/* response */

void Frontend::onResponse(FrontendCSocket client, std::string response)
{
    std::cout << response << std::endl;
}

void Frontend::s_onResponse(ReactorRVal ret)
{
    Packet *packet = (Packet*)(ret.data);
    ((Frontend*)(packet->frontendContext))->onResponse(packet->client, packet->data);
}

/* private */

Frontend& Frontend::operator<<(FrontendBVal add)
{
    DEBUG("frontend: writebuffer: %s", "start");
    if (buffers.find(add.socket) == buffers.end()) {
        DEBUG2("frontend: writebuffer/new: %s", add.data.c_str());
        buffers[add.socket] = add.data;
    } else {
        DEBUG2("frontend: writebuffer/append: %s", add.data.c_str());
        buffers[add.socket] = buffers[add.socket] + add.data;
    }
    if (std::size_t pos = buffers[add.socket].find("\n") != std::string::npos) {
        DEBUG2("frontend: writebuffer/newline: ", "found!");
        Packet *packet = new Packet;
        packet->frontendContext = this;
        packet->client = add.socket;
        packet->data = buffers[add.socket].substr(0, pos);
        ReactorEStruct ev = { .event = Event_On_Set, .data = packet, .length = sizeof(packet) };
        backend->addEvent(ev);
    }
    DEBUG("frontend: writebuffer: %s", "ok");
    return *this;
}

