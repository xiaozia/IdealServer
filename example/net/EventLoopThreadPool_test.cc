
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : EventLoopThreadPool_test.cc
*   Last Modified : 2019-07-05 14:46
*   Describe      :
*
*******************************************************/

#include "ideal/net/EventLoopThreadPool.h"
#include "ideal/net/EventLoop.h"
#include "ideal/base/Thread.h"

#include <assert.h>
#include <stdio.h>
#include <unistd.h>

using namespace ideal;
using namespace ideal::net;

void print(EventLoop* p = nullptr) {
  printf("main(): pid = %d, tid = %d, loop = %p\n",
         getpid(), CurrentThread::tid(), p);
}

void init(EventLoop* p) {
  printf("init(): pid = %d, tid = %d, loop = %p\n",
         getpid(), CurrentThread::tid(), p);
}

int main()
{
	print();

	EventLoop loop;
	loop.runAfter(11, std::bind(&EventLoop::quit, &loop));

	{
		printf("Single thread %p:\n", &loop);
		EventLoopThreadPool model(&loop, "single");
		model.setThreadNum(0);
		model.start(init);
		assert(model.getNextLoop() == &loop);
		assert(model.getNextLoop() == &loop);
		assert(model.getNextLoop() == &loop);
	}

	{
		printf("Another thread:\n");
		EventLoopThreadPool model(&loop, "another");
		model.setThreadNum(1);
		model.start(init);
		EventLoop* nextLoop = model.getNextLoop();
		nextLoop->runAfter(2, std::bind(print, nextLoop));
		assert(nextLoop != &loop);
		assert(nextLoop == model.getNextLoop());
		assert(nextLoop == model.getNextLoop());
		::sleep(3);
	}

	{
		printf("Three threads:\n");
		EventLoopThreadPool model(&loop, "three");
		model.setThreadNum(3);
		model.start(init);
		EventLoop* nextLoop = model.getNextLoop();
		nextLoop->runInLoop(std::bind(print, nextLoop));
		assert(nextLoop != &loop);
		assert(nextLoop != model.getNextLoop());
		assert(nextLoop != model.getNextLoop());
		assert(nextLoop == model.getNextLoop());
	}

	loop.loop();
}



