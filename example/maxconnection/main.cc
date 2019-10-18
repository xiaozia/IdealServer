
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : main.cc
*   Last Modified : 2019-07-12 18:11
*   Describe      :
*
*******************************************************/

#include "example/maxconnection/echo.h"

#include "ideal/base/Logger.h"
#include "ideal/net/EventLoop.h"

#include <unistd.h>

using namespace ideal;
using namespace ideal::net;

int main(int argc, char* argv[]) {
	LOG_INFO << "pid = " << getpid();
	EventLoop loop;
	InetAddress listenAddr(2007);
	int maxConnections = 5;
	if (argc > 1) {
		maxConnections = atoi(argv[1]);
	}
	LOG_INFO << "maxConnections = " << maxConnections;
	EchoServer server(&loop, listenAddr, maxConnections);
	server.start();
	loop.loop();
}

