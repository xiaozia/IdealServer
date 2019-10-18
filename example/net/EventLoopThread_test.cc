
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : EventLoopThread_test.cc
*   Last Modified : 2019-07-05 10:02
*   Describe      :
*
*******************************************************/

#include "ideal/net/EventLoopThread.h"
#include "ideal/net/EventLoop.h"
#include "ideal/base/Thread.h"
#include "ideal/base/CountDownLatch.h"

#include <stdio.h>
#include <unistd.h>

using namespace ideal;
using namespace ideal::net;

void print(EventLoop* p = nullptr) {
	printf("print: pid = %d, tid = %d, loop = %p\n",
         	getpid(), CurrentThread::tid(), p);
}

void quit(EventLoop* p) {
	print(p);
	p->quit();
}

int main()
{
	print();

	{
		EventLoopThread thr1;  // never start
	}

	{
		// dtor calls quit()
		EventLoopThread thr2;
		EventLoop* loop = thr2.startLoop();
		loop->runInLoop(std::bind(print, loop));
		CurrentThread::sleepUsec(500 * 1000);
	}
    // thr2 已经析构，loop的地址有可能相同
	{
		// quit() before dtor
		EventLoopThread thr3;
		EventLoop* loop3 = thr3.startLoop();
	    loop3->runInLoop(std::bind(quit, loop3));
		CurrentThread::sleepUsec(500 * 1000);
	}
}

