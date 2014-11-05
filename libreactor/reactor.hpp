#include <vector>
#include <map>
#include <string>

#define ReactorEE long long
#define ReactorHandler ReactorRVal(*)(void *data, size_t length, Reactor* reactor)
#define ReactorHandlerNamed ReactorRVal(*callback)(void *data, size_t length, Reactor* reactor)
#define ReactorEID long long
#define ZeroEE 0
#define ReactorREmpty { .data = NULL, .length = 0 };

typedef struct {
    ReactorEE event;
    void *data;
    size_t length;
    ReactorEID id;
} ReactorEStruct;

typedef struct {
    void *data;
    size_t length;
} ReactorRVal;

class Reactor {

    public:
        Reactor();
        ReactorRVal addEvent(ReactorEStruct event, bool sync);
        bool addHandler(ReactorEE onEvent, ReactorHandler);
    private:
        void processEvent();
        ReactorEID makeId();

        ReactorEID incremental;

        std::vector<ReactorEStruct> events;
        std::map<ReactorEE, ReactorHandler> handlers;
        std::map<ReactorEID, ReactorRVal> returns;
};
