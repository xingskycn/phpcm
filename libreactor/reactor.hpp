#include <queue>
#include <map>
#include <string>

#define ReactorEE long long
#define ReactorHandler ReactorRVal(*)(void *data, size_t length, Reactor* reactor)
#define ReactorHandlerNamed ReactorRVal(*callback)(void *data, size_t length, Reactor* reactor)
#define ReactorUserCallback void(*)(ReactorRVal ret)
#define ReactorUserCallbackNamed void(*callback)(ReactorRVal ret)
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
        /* object manage */
        Reactor();

        /* public interface */
        ReactorRVal addEventSync(ReactorEStruct event);
        void addEventAsync(ReactorEStruct event, ReactorUserCallback);
        bool addHandler(ReactorEE onEvent, ReactorHandler);
        bool runtime();
    private:
        /* methods */
        void processEvent();
        ReactorEID makeId();

        /* counters */
        ReactorEID incremental;

        /* property */
        std::queue<ReactorEStruct> events;

        std::map<ReactorEE, ReactorHandler> handlers;

        std::map<ReactorEID, ReactorRVal> returns;
        std::map<ReactorEID, ReactorUserCallback> storedCallbacks;
};
