#include <iostream>
#include "pthread.h"

#include "debug.hpp"

#include "frontend.hpp"
#include "backend.hpp"
#include "thread.hpp"

FrontendBVal testCast(std::string str)
{
    FrontendBVal bValA = { .data=str, .length=sizeof(str), .socket=1 };
    return bValA;
}

int main(int argc, char* argv[])
{
    INFO("loader: starting: %s", "loading...");
    Backend *backend = new Backend();
    pthread_t backend_t = runThread(backend);
    Frontend *frontend = new Frontend(backend);
    *frontend << testCast("get A\n");
    *frontend << testCast("set A serialized....\n");
    *frontend << testCast("exec A\n");
    *frontend << testCast("del A\n");
    DEBUG("loader: %s", "runbackend");
    backend->start();
    INFO("loader: exiting: %s", "ok");
    return 0;
}
