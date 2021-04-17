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