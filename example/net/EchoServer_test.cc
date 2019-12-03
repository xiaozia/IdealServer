
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : EchoServer_test.cc
*   Last Modified : 2019-12-01 13:17
*   Describe      :
*
*******************************************************/

#include "ideal/net/TcpServer.h"

#include "ideal/base/Logger.h"
#include "ideal/base/Thread.h"
#include "ideal/net/EventLoop.h"
#include "ideal/net/InetAddress.h"

#include <utility>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

using namespace ideal;
using namespace ideal::net;

int numThreads = 0;

class EchoServer {
public:
    EchoServer(EventLoop* loop, const InetAddress& listenAddr) :
        _loop(loop),
        _server(loop, listenAddr, "EchoServer") {
        _server.setConnectionCallback(
            std::bind(&EchoServer::onConnection, this, std::placeholders::_1));
        _server.setMessageCallback(
            std::bind(&EchoServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        _server.setThreadNum(numThreads);
    }

    void start() {
        _server.start();
    }

private:
    void onConnection(const TcpConnectionPtr& conn) {
        LOG_TRACE << conn->peerAddress().toIpPort() << " -> "
                  << conn->localAddress().toIpPort() << " is "
                  << (conn->connected() ? "UP" : "DOWN");
        LOG_INFO << conn->getTcpInfoString();
        conn->send("hello\n");
    }

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time) {
        std::string msg(buf->retrieveAllAsString());
        LOG_TRACE << conn->name() << " recv " << msg.size() << " bytes at " << time.toString();
        if(msg == "exit\n") {
            conn->send("bye\n");
            conn->shutdown();
        }
        if (msg == "quit\n") {
            _loop->quit();
        }
        conn->send(msg);
    }

private:
    EventLoop* _loop;
    TcpServer _server;
};

int main(int argc, char* argv[])
{
	LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
 	LOG_INFO << "sizeof TcpConnection = " << sizeof(TcpConnection);

  	if(argc > 1) {
    	numThreads = atoi(argv[1]);
  	}
  
	EventLoop loop;
  	InetAddress listenAddr(2000, false);
  	EchoServer server(&loop, listenAddr);
    server.start();
    loop.loop();
}

