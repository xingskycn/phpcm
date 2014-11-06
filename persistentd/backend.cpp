#include "reactor.hpp"
#include "debug.hpp"

#include "backend.hpp"

Backend::Backend()
{
    reactor = new Reactor();
    reactor->addHandler(Event_On_Set, this->);
    reactor->addHandler(Event_On_Del);
    reactor->addHandler(Event_On_Get);
    reactor->addHandler(Event_On_Exec);
}

void Backend::setCallback(BackendResponderNamed)
{
    this->callback = callback;
}

void* Backend::start()
{
    INFO("backend: starting: %s", "start...");
    INFO("backend: starting: %s", "ok");
}

void Backend::addEvent(ReactorEStruct event)
{
    reactor->addEventAsync(event, this->callback);
}

