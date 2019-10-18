add_executable(maxconnection_echo main.cc echo.cc)
target_link_libraries(maxconnection_echo ideal_net ideal_base pthread)
