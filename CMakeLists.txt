cmake_minimum_required(VERSION 3.10)

project(Spawned)

add_library(Spawn SyncSpawn.cpp)

add_executable(spawnExample main.cpp)

target_link_libraries(spawnExample Spawn)