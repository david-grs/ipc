cmake_minimum_required(VERSION 2.8)

add_definitions("-std=c++1z")

add_executable(server server.cc)
target_link_libraries(server pthread rt)

add_executable(client client.cc)
target_link_libraries(client pthread rt)

