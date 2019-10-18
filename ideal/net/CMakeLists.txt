set(NET_SRCS
    InetAddress.cc
    SocketUtil.cc
    Socket.cc
    Timer.cc
    TimerQueue.cc
    EventLoop.cc
    Channel.cc
    Poller.cc
    poller/DefaultPoller.cc
    poller/EpollPoller.cc
    EventLoopThread.cc
    EventLoopThreadPool.cc
    Acceptor.cc
    Buffer.cc
    TcpConnection.cc
    TcpServer.cc
    Connector.cc
    TcpClient.cc
   )

add_library(ideal_net ${NET_SRCS})

install(TARGETS ideal_net DESTINATION ${PROJECT_SOURCE_DIR}/lib)

set(NET_HEADERS
    Endian.h
    InetAddress.h
    SocketUtil.h
    Socket.h
    Timer.h
    TimerId.h
    Callbacks.h
    TimerQueue.h
    EventLoop.h
    Channel.h
    Poller.h
    poller/EpollPoller.h
    EventLoopThread.h
    EventLoopThreadPool.h
    Acceptor.h
    Buffer.h
    TcpConnection.h
    TcpServer.h
    Connector.h
    TcpClient.h
   )


install(FILES ${NET_HEADERS} DESTINATION ${PROJECT_SOURCE_DIR}/include/ideal/net)

