cmake_minimum_required(VERSION 2.8)

add_definitions("-std=c++1z")

add_executable(ipc main.cc)
target_link_libraries(ipc pthread rt)
