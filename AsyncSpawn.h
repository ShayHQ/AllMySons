#include "Spawn.h"
#include <functional>
#include <future>

namespace spawnchild{
    struct AsyncSpawnEvents{
        std::function<void(std::string)> onMessage = [](std::string){};
        std::function<void(std::string)> onError = [](std::string){};
        std::function<void(int, bool)> onExit = [](int code, bool crashed){};
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
        void waitEvents();
        void killProcess();
    protected:
        void onExit();
        void onMessage();
        void onError();

        void connectEvents();
    };
}