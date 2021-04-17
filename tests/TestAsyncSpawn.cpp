#include "../AsyncSpawn.h"
#include <unistd.h>
#include <catch2/catch_all.hpp>

using namespace spawnchild;


TEST_CASE("Async Spawn ", "[AsyncSpawn]"){
    std::vector<std::string> args;
    std::string processPath = "./child";
    AsyncSpawnEvents events;

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

    events.onMessage = [](std::string data){
        std::string got = data;
    };
    AsyncSpawn childProg(processPath, args, events);

    childProg.send("12");

    childProg.send("0");

    while (!finished);

    REQUIRE(isExited);
}