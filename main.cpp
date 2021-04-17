#include "SyncSpawn.h"
#include <iostream>

int main(){
    std::string processPath = "ls";
    std::vector<std::string> args = {"-la"};
    SyncSpawn syncSpawned(processPath, args);
    std::cout << syncSpawned.getResult();
    std::cout << syncSpawned.getResult();
    return 0;
}