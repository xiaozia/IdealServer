
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : echo.cc
*   Last Modified : 2019-07-12 18:11
*   Describe      :
*
*******************************************************/

#include "example/maxconnection/echo.h"

#include "ideal/base/Logger.h"

using namespace ideal;
using namespace ideal::net;

EchoServer::EchoServer(EventLoop* loop,
                       const InetAddress& listenAddr,
                       int maxConnections) :
	_server(loop, listenAddr, "EchoServer"),
    _numConnected(0),
    _kMaxConnections(maxConnections) {
  	_server.setConnectionCallback(std::bind(&EchoServer::onConnection, this, _1));
  	_server.setMessageCallback(std::bind(&EchoServer::onMessage, this, _1, _2, _3));
}

void EchoServer::start() {
  	_server.start();
}

void EchoServer::onConnection(const TcpConnectionPtr& conn) {
	LOG_INFO << "EchoServer - " << conn->peerAddress().toIpPort() << " -> "
             << conn->localAddress().toIpPort() << " is "
             << (conn->connected() ? "UP" : "DOWN");

  	if(conn->connected()) {
    	++_numConnected;
    	if(_numConnected > _kMaxConnections) {
    		conn->shutdown();
    		conn->forceCloseWithDelay(3.0);  // > round trip of the whole Internet.
    	}
  	}
  	else {
    	--_numConnected;
  	}
  	LOG_INFO << "numConnected = " << _numConnected;
}

void EchoServer::onMessage(const TcpConnectionPtr& conn,
                           Buffer* buf,
                           Timestamp time) {
  	std::string msg(buf->retrieveAllAsString());
  	LOG_INFO << conn->name() << " echo " << msg.size() << " bytes at " << time.toString();
  	conn->send(msg);
}

