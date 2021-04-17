cmake_minimum_required(VERSION 3.10)

project(Spawned)

add_library(SpawnChild Spawn.cpp StdioPipe.cpp SyncSpawn AsyncSpawn)

target_link_libraries(SpawnChild pthread)

add_executable(spawnExample main.cpp)

target_link_libraries(spawnExample SpawnChild)

add_executable(child child.cpp)

set(SpawnChild_LIBS SpawnChild)
set(SpawnChild_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})