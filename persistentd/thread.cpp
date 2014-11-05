#include "thread.hpp"

pthread_t runThread(IServerThread *tObj)
{
    pthread_t ret;
    pthread_create(&ret, NULL, &IServerThread::s_start, tObj);
    return ret;
}
