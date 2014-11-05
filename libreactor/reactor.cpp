#include "reactor.hpp"

/*
**** Object manage
*/

Reactor::Reactor()
{
    incremental=0;
}

/*
**** Public
*/

ReactorRVal Reactor::addEventSync(ReactorEStruct event)
{
    ReactorRVal empty = ReactorREmpty;
    event.id = makeId();
    returns[event.id] = empty;
    events.push(event);
    while ((returns[event.id].data == NULL)&&(!events.empty())) { //while event has not response
        processEvent();
    }
    ReactorRVal ret = returns[event.id];
    return ret;
}

void Reactor::addEventAsync(ReactorEStruct event, ReactorUserCallbackNamed)
{
    ReactorRVal empty = ReactorREmpty;
    event.id = makeId();
    returns[event.id] = empty;
    storedCallbacks[event.id] = callback;
    events.push(event);
}

bool Reactor::addHandler(ReactorEE onEvent, ReactorHandlerNamed)
{
    //event already listen
    if (handlers.find(onEvent) != handlers.end()) return false;
    
    handlers[onEvent] = callback;
    return true;
}

bool Reactor::runtime()
{
    bool processingBeen = !events.empty();
    while (!events.empty()) {
        processEvent();
    }
    return processingBeen;
}

/*
**** Private
*/

ReactorEID Reactor::makeId()
{
    return ++incremental;
}

void Reactor::processEvent()
{
    ReactorEStruct event = events.front();
    events.pop();
    //event has not handler
    if (handlers.find(event.event) == handlers.end()) {
//        WARNING(PREFIX("event %lld has not handler"), event.event);
        return;
    }
    ReactorRVal retval = handlers[event.event](event.data, event.length, this);
    if (storedCallbacks.find(event.id) != storedCallbacks.end()) {
        storedCallbacks[event.id](retval);
        storedCallbacks.erase(event.id);
    } else {
        returns[event.id] = retval;
    }
}
