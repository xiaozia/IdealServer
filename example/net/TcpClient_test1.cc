
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : TcpClient_test1.cc
*   Last Modified : 2019-07-12 14:04
*   Describe      :
*
*******************************************************/

#include "ideal/base/Logger.h"
#include "ideal/net/EventLoop.h"
#include "ideal/net/TcpClient.h"

using namespace ideal;
using namespace ideal::net;

TcpClient* g_client;

void timeout()
{
	LOG_INFO << "timeout";
	g_client->stop();
}

int main(int argc, char* argv[])
{
	EventLoop loop;
	InetAddress serverAddr("127.0.0.1", 2); // no such server
	TcpClient client(&loop, serverAddr, "TcpClient");
	g_client = &client;
	loop.runAfter(0.0, timeout);
	loop.runAfter(1.0, std::bind(&EventLoop::quit, &loop));
	client.connect();
	CurrentThread::sleepUsec(100 * 1000);
	loop.loop();
}



