#include "../SyncSpawn.h"
#include <catch2/catch_all.hpp>

#ifdef WIN32
#define CHILD_PROCESS_PATH "./Debug/child.exe"
#else
#define CHILD_PROCESS_PATH "./child"
#endif
using namespace spawnchild;

TEST_CASE("Sync Spawn ", "[SyncSpawn]"){
    std::vector<std::string> args;
    std::string processPath = CHILD_PROCESS_PATH;
    std::string input = "23 0";

    SyncSpawn syncChild(processPath, args, input);
    int indexFound = syncChild.getResult().rfind("Thanks for not exiting 23");
    REQUIRE( indexFound > -1);
}