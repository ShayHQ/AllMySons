cmake_minimum_required(VERSION 3.10)

project(Spawned)

add_subdirectory(opt/Catch2)

add_library(SpawnChild Spawn.cpp StdioPipe.cpp SyncSpawn AsyncSpawn)

target_link_libraries(SpawnChild pthread)

set(SpawnChild_LIBS SpawnChild)
set(SpawnChild_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(tests)