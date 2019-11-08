add_executable(pingpong_server server.cc)
target_link_libraries(pingpong_server ideal_net ideal_base pthread)

add_executable(pingpong_client client.cc)
target_link_libraries(pingpong_client ideal_net ideal_base pthread)

