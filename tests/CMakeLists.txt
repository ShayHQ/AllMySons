cmake_minimum_required(VERSION 3.10)

# if (NOT EXISTS ${DISABLE_TESTS})
enable_testing()

include_directories(../opt/Catch2/src)

add_executable(child child.cpp)

add_executable(TestsSync TestsSync.cpp)
add_executable(TestAsyncSpawn TestAsyncSpawn.cpp)

target_link_libraries(TestAsyncSpawn Catch2WithMain SpawnChild)
target_link_libraries(TestsSync Catch2WithMain SpawnChild)

add_test(TestsSync TestsSync)
add_test(TestAsyncSpawn TestAsyncSpawn)
# endif()

