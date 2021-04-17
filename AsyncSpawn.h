#include "Spawn.h"
#include <functional>
#include <future>

struct AsyncSpawnEvents{
    std::function<void(std::string)> onMessage;
    std::function<void(std::string)> onError;
    std::function<void(int, bool)> onExit;
};

class AsyncSpawn : public Spawn
{
private:
    std::vector<std::future<void>> eventListeners;
    bool stop = false;
    AsyncSpawnEvents events;
public:
    AsyncSpawn(std::string&, std::vector<std::string>&, AsyncSpawnEvents);
    ~AsyncSpawn();
    void send(std::string);
protected:
    void onExit();
    void onMessage();
    void onError();

    void killProcess();
    void waitEvents();
    void connectEvents();
};