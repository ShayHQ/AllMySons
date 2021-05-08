#include "../AsyncSpawn.h"

#include <catch2/catch_all.hpp>
#ifdef WIN32
#define CHILD_PROCESS_PATH "./Debug/child.exe"
#else
#define CHILD_PROCESS_PATH "./child"
#endif
using namespace spawnchild;

#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
TEST_CASE("Async Spawn ", "[AsyncSpawn]"){
    std::vector<std::string> args;
    std::string processPath = CHILD_PROCESS_PATH;
    AsyncSpawnEvents events;
    std::string res = "";

    bool finished = false;
    bool isExited = false;

    events.onExit = [&isExited, &finished](int code, bool crashed){
        std::mutex mtx;
        mtx.lock();
        if (!crashed){
            isExited = true;
        }
        finished = true;
        mtx.unlock();
    };

    events.onMessage = [&res](std::string data){
        res += data;
    };
    AsyncSpawn childProg(processPath, args, events);

    childProg.send("12");

    childProg.send("0");

    childProg.waitEvents();
    REQUIRE((isExited && res.size() > 0));
}