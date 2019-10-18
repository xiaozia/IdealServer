
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : TimerQueue_test.cc
*   Last Modified : 2019-06-22 14:23
*   Describe      :
*
*******************************************************/

#include "ideal/net/EventLoop.h"
#include "ideal/net/EventLoopThread.h"
#include "ideal/base/Thread.h"
#include "ideal/net/Timer.h"

#include <iostream>
#include <stdio.h>
#include <unistd.h>

using namespace ideal;
using namespace ideal::net;

int cnt = 0;
EventLoop* g_loop;

void printTid() {
	printf("pid = %d, tid = %d\n", getpid(), CurrentThread::tid());
	printf("now %s\n", Timestamp::now().toString().c_str());
}

void print(const char* msg) {
	printf("msg %s %s\n", Timestamp::now().toString().c_str(), msg);
	if(++cnt == 20) {
    	g_loop->quit();
  	}
}

void cancel(TimerId timer) {
	g_loop->cancel(timer);
	printf("cancelled at %s\n", Timestamp::now().toString().c_str());
}

int main() {
	printTid();
	sleep(1);
	{
		EventLoop loop;
		g_loop = &loop;

		print("main");
        std::cout << "1 " << Timer::numCreated() << std::endl;
		loop.runAfter(1, std::bind(print, "once1"));
		std::cout << "2 " << Timer::numCreated() << std::endl;
        loop.runAfter(1.5, std::bind(print, "once1.5"));
		loop.runAfter(2.5, std::bind(print, "once2.5"));
		loop.runAfter(3.5, std::bind(print, "once3.5"));
	    std::cout << "3 " << Timer::numCreated() << std::endl;
        TimerId t45 = loop.runAfter(4.5, std::bind(print, "once4.5"));
		std::cout << "4 " << Timer::numCreated() << std::endl;
        loop.runAfter(4.2, std::bind(cancel, t45));
		loop.runAfter(4.8, std::bind(cancel, t45));
		std::cout << "5 " << Timer::numCreated() << std::endl;
        loop.runEvery(2, std::bind(print, "every2"));
		std::cout << "6 " << Timer::numCreated() << std::endl;
        TimerId t3 = loop.runEvery(3, std::bind(print, "every3"));
		loop.runAfter(9.001, std::bind(cancel, t3));
        std::cout << "7 " << Timer::numCreated() << std::endl;

		loop.loop();
		print("main loop exits");
	}

	sleep(1);
	{
		EventLoopThread loopThread;
		EventLoop* loop = loopThread.startLoop();
		loop->runAfter(2, printTid);
		sleep(3);
		print("thread loop exits");
	}

}

