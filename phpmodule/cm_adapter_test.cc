#include <unistd.h>

#include "cm_class.hpp"
#include "cm_ServerPair.hpp"

int main(int argc, char* argv[])
{
    const char *key = "a";
    char *ret;
    size_t size;
    Cm* cm;
    std::vector<ServerPair> configuration;
    ServerPair config = { .serverName = "127.0.0.1", .port=11211, .stable = true, .isReplica=false };
    configuration.push_back(config);
    while (true) {
        cm = new Cm(configuration);
        ret = cm->get((char *)key, &size);
        sleep(1);
        delete cm;
    }
}
