cmake_minimum_required(VERSION 2.8)

add_definitions("-O0 -g -std=c++1z -Wno-deprecated-declarations -Wno-deprecated ")

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(simple)

add_executable(server server.cc)
target_link_libraries(server pthread rt)

add_executable(client client.cc)
target_link_libraries(client pthread rt)

