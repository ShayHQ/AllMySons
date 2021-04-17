cmake_minimum_required(VERSION 3.10)

project(Spawned)

add_library(Spawn Spawn.cpp StdioPipe.cpp SyncSpawn AsyncSpawn)

add_executable(spawnExample main.cpp)

target_link_libraries(spawnExample Spawn pthread)

add_executable(child child.cpp)