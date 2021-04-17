#include "../SyncSpawn.h"
#include <catch2/catch_all.hpp>

using namespace spawnchild;

TEST_CASE("Sync Spawn ", "[SyncSpawn]"){
    std::vector<std::string> args;
    std::string processPath = "./child";
    std::string input = "23 0";

    SyncSpawn syncChild(processPath, args, input);
    int indexFound = syncChild.getResult().rfind("Thanks for not exiting 23");
    REQUIRE( indexFound > -1);
}