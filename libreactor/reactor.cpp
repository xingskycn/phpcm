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

ReactorRVal Reactor::addEvent(ReactorEStruct event, bool sync)
{
    ReactorRVal empty = ReactorREmpty;
    event.id = makeId();
    returns[event.id] = empty;
    events.push_back(event);
    if (sync) {
        while (returns[event.id].data == NULL) { //while event has not response
            processEvent();
        }
    }
    ReactorRVal ret = returns[event.id];
    //delete returns[event.id];
    return ret;
}

bool Reactor::addHandler(ReactorEE onEvent, ReactorHandlerNamed)
{
    //event already listen
    if (handlers.find(onEvent) != handlers.end()) return false;
    
    handlers[onEvent] = callback;
    return true;
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
    ReactorEStruct event = events.back(); //first and remove first
    events.pop_back();
    //event has not handler
    if (handlers.find(event.event) == handlers.end()) {
//        WARNING(PREFIX("event %lld has not handler"), event.event);
        return;
    }
    ReactorRVal retval = handlers[event.event](event.data, event.length, this);
    returns[event.id] = retval;
}
