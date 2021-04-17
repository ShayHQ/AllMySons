# SpawnChild

written in C++,
tested in Linux environment

use cmake to integrate into your project

```
add_subdirectory(spawnchild)
add_target(spawned spawned.cpp)
target_link_libraries(spawned ${SpawnChild_LIBS})

```