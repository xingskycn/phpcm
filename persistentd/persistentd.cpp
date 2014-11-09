#include "pthread.h"

#include "debug.hpp"

#include "storage.hpp"
#include "frontend.hpp"
#include "backend.hpp"
#include "thread.hpp"

int main(int argc, char* argv[])
{
    INFO("loader: starting: %s", "loading...");
    Storage *storage = new Storage();
    Backend *backend = new Backend(storage);
    Frontend *frontend = new Frontend(backend);
    pthread_t t1 = runThread(frontend);
    pthread_t t2 = runThread(backend);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    INFO("loader: exiting: %s", "ok");
    return 0;
}
