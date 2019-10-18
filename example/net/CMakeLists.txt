add_executable(timerqueue_test TimerQueue_test.cc) 
target_link_libraries(timerqueue_test ideal_net ideal_base pthread)

add_executable(eventloopthread_test EventLoopThread_test.cc) 
target_link_libraries(eventloopthread_test ideal_net ideal_base pthread)

add_executable(eventloopthreadpool_test EventLoopThreadPool_test.cc) 
target_link_libraries(eventloopthreadpool_test ideal_net ideal_base pthread)

add_executable(channel_test Channel_test.cc) 
target_link_libraries(channel_test ideal_net ideal_base)

add_executable(echoserver_test EchoServer_test.cc) 
target_link_libraries(echoserver_test ideal_net ideal_base pthread)

add_executable(echoclient_test EchoClient_test.cc) 
target_link_libraries(echoclient_test ideal_net ideal_base)

add_executable(tcpclient_test1 TcpClient_test1.cc) 
target_link_libraries(tcpclient_test1 ideal_net ideal_base)

add_executable(tcpclient_test2 TcpClient_test2.cc) 
target_link_libraries(tcpclient_test2 ideal_net ideal_base pthread)

add_executable(tcpclient_test3 TcpClient_test3.cc) 
target_link_libraries(tcpclient_test3 ideal_net ideal_base pthread)

if(BOOSTTEST_LIBRARY)
    add_executable(inetaddress_test InetAddress_test.cc) 
    target_link_libraries(inetaddress_test ideal_net ideal_base boost_unit_test_framework)
    add_executable(buffer_test Buffer_test.cc) 
    target_link_libraries(buffer_test ideal_net ideal_base boost_unit_test_framework)
endif()
