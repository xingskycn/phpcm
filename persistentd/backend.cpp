#include "reactor.hpp"
#include "debug.hpp"

#include "backend.hpp"

Backend::Backend()
{

}

void Backend::setCallback(BackendResponderNamed)
{

}

void* Backend::start()
{
    INFO("backend: starting: %s", "start...");
    INFO("backend: starting: %s", "ok");
}

void addEvent(ReactorEStruct event)
{

}
