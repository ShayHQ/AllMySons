cmake_minimum_required(VERSION 3.10)

project(Spawned)

message(${CMAKE_CURRENT_SOURCE_DIR})
message(${CMAKE_SOURCE_DIR})

if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    add_subdirectory(opt/Catch2)
    add_subdirectory(tests)
endif()

add_library(SpawnChild Spawn.cpp StdioPipe.cpp SyncSpawn AsyncSpawn)

if(UNIX)
target_link_libraries(SpawnChild pthread)
endif(UNIX)

set(SpawnChild_LIBS SpawnChild)
set(SpawnChild_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})