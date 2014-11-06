#ifndef Packet_HPP
#define Packet_HPP

typedef struct {
    void* frontendContext;
    int client; //socket
    std::string data;
} Packet;

#endif
