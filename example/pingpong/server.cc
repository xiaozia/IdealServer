
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : server.cc
*   Last Modified : 2019-11-17 15:32
*   Describe      :
*
*******************************************************/

#include "ideal/base/Logger.h"
#include "ideal/net/EventLoop.h"
#include "ideal/net/TcpServer.h"

#include <cstdio>
#include <unistd.h>

using namespace ideal;
using namespace ideal::net;

void onConnection(const TcpConnectionPtr& conn) {
    if(conn->connected()) {
        conn->setTcpNoDelay(true);
    }
}

void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp) {
    conn->send(buf);
}

int main(int argc, char* argv[]) {
    if(argc < 4) {
        fprintf(stderr, "Usage: server <address> <port> <threads>\n");
    }
    else {
        Logger::setLogLevel(Logger::WARN);
        LOG_INFO << "pid = " << ::getpid() << ", tid = " << CurrentThread::tid();
        
        const char* ip = argv[1];
        uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
        InetAddress listenAddr(ip, port);
        int threadNum = atoi(argv[3]);

        EventLoop loop;
        TcpServer server(&loop, listenAddr, "PingPong");

        if(threadNum > 1) {
            server.setThreadNum(threadNum);
        }

        server.setConnectionCallback(onConnection);
        server.setMessageCallback(onMessage);

        server.start();
        loop.loop();
    }
}

