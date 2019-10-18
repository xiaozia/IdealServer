
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : echo.h
*   Last Modified : 2019-07-12 18:11
*   Describe      :
*
*******************************************************/

#ifndef  _EXAMPLE_MAXCONNECTION_ECHO_H
#define  _EXAMPLE_MAXCONNECTION_ECHO_H

#include "ideal/net/TcpServer.h"

class EchoServer {
public:
	EchoServer(ideal::net::EventLoop* loop,
               const ideal::net::InetAddress& listenAddr,
               int maxConnections);
	void start();

private:
	void onConnection(const ideal::net::TcpConnectionPtr& conn);
	void onMessage(const ideal::net::TcpConnectionPtr& conn,
                   ideal::net::Buffer* buf,
                   ideal::Timestamp time);

private:
  	ideal::net::TcpServer _server;
  	int _numConnected;
  	const int _kMaxConnections;
};

#endif // _EXAMPLE_MAXCONNECTION_ECHO_H

