
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : TcpClient_test3.cc
*   Last Modified : 2019-08-01 16:48
*   Describe      :
*
*******************************************************/

// TcpClient destructs in a different thread.

#include "ideal/base/Logger.h"
#include "ideal/base/CurrentThread.h"
#include "ideal/net/EventLoopThread.h"
#include "ideal/net/TcpClient.h"

using namespace ideal;
using namespace ideal::net;

int main(int argc, char* argv[]) {
  	Logger::setLogLevel(Logger::TRACE);

  	EventLoopThread loopThread;
  	{
  		InetAddress serverAddr("127.0.0.1", 1234); // should succeed
  		TcpClient client(loopThread.startLoop(), serverAddr, "TcpClient");
 		client.connect();
  		CurrentThread::sleepUsec(500 * 1000);  // wait for connect
  		client.disconnect();
  	}

  	CurrentThread::sleepUsec(1000 * 1000);
}

