#include <iostream>
#include <unistd.h>
#include "pthread.h"

#include "debug.hpp"

#include "storage.hpp"
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
    Storage *storage = new Storage();
    Backend *backend = new Backend(storage);
    pthread_t backend_t = runThread(backend);
    Frontend *frontend = new Frontend(backend);
    *frontend << testCast("set A serialized....\n");
    DEBUG("loader: %s", "runbackend");
//    backend->start();
    pthread_join(backend_t, NULL);
    INFO("loader: exiting: %s", "ok");
    return 0;
}
