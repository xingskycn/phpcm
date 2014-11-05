#include <iostream>

#include "reactor.hpp"

ReactorRVal handler2(void *data, size_t length, Reactor *reactor)
{
    std::cout << "handler2" << std::endl;
    ReactorRVal r;
    r.data = new char [1];
    r.length = 1;
    std::cout << "return2" << std::endl;
    return r;
}

ReactorRVal handler(void *data, size_t length, Reactor *reactor)
{
    std::cout << "handler" << std::endl;
    ReactorEStruct event = { .event = 2, .data=NULL, .length=0  };
    reactor->addEvent(event, false);

    ReactorRVal r;
    r.data = new char [1];
    r.length = 1;
    std::cout << "return" << std::endl;
    return r;
}

int main(int argc, char* argv[])
{
    Reactor *reactor = new Reactor();
    reactor->addHandler(1, &handler);
    reactor->addHandler(2, &handler2);
    ReactorEStruct event = { .event = 1, .data=NULL, .length=0  };
    reactor->addEvent(event, false);
    reactor->addEvent(event, false);
    reactor->addEvent(event, true);
}
