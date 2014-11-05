#include "debug.hpp"

#include "frontend.hpp"

/* public interface */

void* Frontend::start()
{
    INFO("frontend: starting: %s", "start...");
    FrontendBVal add = { .data = (char *)"str", .length=sizeof("str") };
    *this << add;
    INFO("frontend: starting: %s", "ok");
}

/* private */

Frontend& Frontend::operator<<(FrontendBVal add)
{
    DEBUG("frontend: writebuffer: %s", "start");
    DEBUG2("frontend: writebuffer: %s", add.data);
    DEBUG("frontend: writebuffer: %s", "ok");
}

