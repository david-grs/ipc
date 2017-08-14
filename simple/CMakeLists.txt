add_executable(simple_server server.cc)
target_link_libraries(simple_server pthread rt)

add_executable(simple_client client.cc)
target_link_libraries(simple_client pthread rt)

