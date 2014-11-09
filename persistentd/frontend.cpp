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
//    std::cout << response << std::endl;
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
    std::size_t pos = buffers[add.socket].find("\n");
    while (pos != std::string::npos) {
        DEBUG2("frontend: writebuffer/newline: ", "found!");
        std::string cmdline = buffers[add.socket].substr(0, pos);
        std::size_t space_pos = cmdline.find(" ");
        std::string cmd = buffers[add.socket].substr(0, space_pos);
        std::string args = cmdline.substr(space_pos+1);

        ReactorEE event;
        if (cmd.compare("get") == 0) {
            event = Event_On_Get;
        } else if (cmd.compare("set") == 0) {
            event = Event_On_Set;
        } else if (cmd.compare("del") == 0) {
            event = Event_On_Del;
        } else if (cmd.compare("exec") == 0) {
            event = Event_On_Exec;
        } else {
            event = 100;
        }

        Packet *packet = new Packet;
        packet->frontendContext = this;
        packet->client = add.socket;
        packet->data = args;
        ReactorEStruct ev = { .event = event, .data = packet, .length = sizeof(packet) };
        backend->addEvent(ev);
        buffers[add.socket] = buffers[add.socket].substr(pos+1);
        pos = buffers[add.socket].find("\n");
    }
    DEBUG("frontend: writebuffer: %s", "ok");
    return *this;
}

