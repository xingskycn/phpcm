
#ifndef IServerThread_HPP
#define IServerThread_HPP

class IServerThread
{
    public:
        virtual void* start(void)=0;
        static void* s_start(void *context) {
            return ((IServerThread*)context)->start();
        };
};

#endif
