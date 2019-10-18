
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : TcpClient_test2.cc
*   Last Modified : 2019-08-01 15:31
*   Describe      :
*
*******************************************************/

// TcpClient destructs when TcpConnection is connected but unique.

#include "ideal/base/Logger.h"
#include "ideal/base/Thread.h"
#include "ideal/net/EventLoop.h"
#include "ideal/net/TcpClient.h"

using namespace ideal;
using namespace ideal::net;

void threadFunc(EventLoop* loop) {
	InetAddress serverAddr("127.0.0.1", 1234); // should succeed
	TcpClient client(loop, serverAddr, "TcpClient");
	client.connect();

  	CurrentThread::sleepUsec(1000*1000);
  	// client destructs when connected.
}

int main(int argc, char* argv[]) {
	Logger::setLogLevel(Logger::DEBUG);
  
	EventLoop loop;
  	loop.runAfter(3.0, std::bind(&EventLoop::quit, &loop));
  	Thread thr(std::bind(threadFunc, &loop));
  	thr.start();
  	loop.loop();
}

