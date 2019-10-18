
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : ThreadPool_test.cc
*   Last Modified : 2019-06-06 15:12
*   Describe      :
*
*******************************************************/

#include "ideal/base/ThreadPool.h"
#include "ideal/base/CountDownLatch.h"
#include "ideal/base/CurrentThread.h"
#include "ideal/base/Logger.h"

#include <stdio.h>
#include <unistd.h>

void print() {
    printf("tid = %d\n", ideal::CurrentThread::tid());
}

void printString(const std::string& str) {
    LOG_INFO << str;
    usleep(100*1000);
}

void test(int maxSize) {
    LOG_WARN << "Test ThreadPool with max queue size = " << maxSize;
    ideal::ThreadPool pool("MainThreadPool");
    pool.setMaxQueueSize(maxSize);
    pool.start(5);

    LOG_WARN << "Adding";
    pool.run(print);
    pool.run(print);
    for (int i = 0; i < 100; ++i) {
        char buf[32];
        snprintf(buf, sizeof buf, "task %d", i);
        pool.run(std::bind(printString, std::string(buf)));
    }
    LOG_WARN << "Done";

    ideal::CountDownLatch latch(1);
    pool.run(std::bind(&ideal::CountDownLatch::countDown, &latch));
    latch.wait();
    pool.stop();
}

/*
 * Wish we could do this in the future.
void testMove()
{
  muduo::ThreadPool pool;
  pool.start(2);

  std::unique_ptr<int> x(new int(42));
  pool.run([y = std::move(x)]{ printf("%d: %d\n", muduo::CurrentThread::tid(), *y); });
  pool.stop();
}
*/

int main() {
    test(0);
    test(1);
    test(5);
    test(10);
    test(50);
}



