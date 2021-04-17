#include "AsyncSpawn.h"
#include "SyncSpawn.h"
#include <iostream>

int main(){
    std::string input = "2332\n";
    std::vector<std::string> args;
    std::string processPath = "./child";
    AsyncSpawnEvents events;

    events.onExit = [](int code, bool crashed){
        if (crashed){
            std::cout << "child terminated with signal " << code << "\n";
        }else{
            std::cout << "child exited with " << code << "\n";
        }
    };
    events.onMessage = [](std::string data){
        std::cout << data << "\n";
    };
    events.onError = [](std::string data){
        std::cout << "Error: " << data << "\n";
    };
    AsyncSpawn childProg(processPath, args, events);

    childProg.send(input);

    childProg.send(input);

    while (childProg.isAlive());
    
    return 0;
}