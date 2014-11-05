#include <iostream>
#include <string.h>

#include "reactor.hpp"

void cb(ReactorRVal ret)
{
    std::cout << "cb-async=" << (char*)ret.data << std::endl;
}

ReactorRVal handler2(void *data, size_t length, Reactor *reactor)
{
    std::cout << "handler2" << std::endl;
    ReactorRVal r;
    r.data = new char [5];
    strcpy((char*)r.data, "retu");
    r.length = 5;
    std::cout << "return2" << std::endl;
    return r;
}

ReactorRVal handler(void *data, size_t length, Reactor *reactor)
{
    std::cout << "handler: " << (const char *)data << std::endl;
    ReactorEStruct event = { .event = 2, .data=NULL, .length=0  };
    reactor->addEventAsync(event, &cb);

    ReactorRVal r;
    r.data = new char [5];
    strcpy((char*)r.data, "retu");
    r.length = 5;
    std::cout << "return" << std::endl;
    return r;
}

int main(int argc, char* argv[])
{
    Reactor *reactor = new Reactor();
    reactor->addHandler(1, &handler);
    reactor->addHandler(2, &handler2);
    ReactorEStruct event1 = { .event = 1, .data = (char*)"str1", .length=sizeof("str1") };
    reactor->addEventAsync(event1, &cb);
    ReactorEStruct event2 = { .event = 1, .data = (char*)"str2", .length=sizeof("str2") };
    reactor->addEventAsync(event2, &cb);
    ReactorEStruct event3 = { .event = 1, .data = (char*)"str3", .length=sizeof("str3") };
    reactor->addEventSync(event3);
    reactor->runtime();
}
