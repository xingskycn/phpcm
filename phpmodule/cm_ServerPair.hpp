
#ifndef CM_CM_ServerPair_H
#define CM_CM_ServerPair_H

struct ServerPair_t {
    const char* serverName;
    int port;
    bool stable;
    bool isReplica;
    std::vector<struct ServerPair_t> replica;
};

typedef ServerPair_t ServerPair;

#endif
