# SpawnChild

written in C++,
tested in Linux environment

use cmake to integrate into your project

```
add_subdirectory(spawnchild)
add_target(spawned spawned.cpp)
target_link_libraries(spawned ${SpawnChild_LIBS})
```

Sync use example:

```c++
#include <SpawnChild.h>

std::string input = "";
std::vector<std::string> args = {"-la"};
std::string processPath = "ls";

spawnchild::SyncSpawn lsSpawned(processPath, args, input);

std::string result = lsSpawned.getResult();
```

Async event driven use example:

```c++
#include <mutex>
#include <SpawnChild.h>

std::vector<std::string> args;
std::string processPath = "./child";
spawnchild::AsyncSpawnEvents events;

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
spawnchild::AsyncSpawn childProg(processPath, args, events);

childProg.send("12");

childProg.send("0");

while (!finished);

// child is exited

```