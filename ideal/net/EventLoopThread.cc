
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : EventLoopThread.cc
*   Last Modified : 2019-11-23 14:11
*   Describe      :
*
*******************************************************/

#include <ideal/net/EventLoopThread.h>
#include <ideal/net/EventLoop.h>

#include <assert.h>

using namespace ideal;
using namespace ideal::net;

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb, const std::string& name) : 
    _loop(nullptr),
    _exiting(false),
    _thread(std::bind(&EventLoopThread::threadFunc, this), name),
    _mtx(),
    _cv(),
    _callback(cb) {
}

EventLoopThread::~EventLoopThread() {
    _exiting = true;
    if(_loop != nullptr) {
        _loop->quit();
        _thread.join();
    }
}

EventLoop* EventLoopThread::startLoop() {
    assert(!_thread.started());
    _thread.start();

    EventLoop* loop = nullptr;
    {
        std::unique_lock<std::mutex> ul(_mtx);
        _cv.wait(ul, [this](){ return _loop != nullptr; });
        loop = _loop;
    }
    
    return loop;
}

void EventLoopThread::threadFunc() {
    EventLoop loop;
    if(_callback) {
        _callback(&loop);
    }

    {
        std::lock_guard<std::mutex> lg(_mtx);
        _loop = &loop;
        _cv.notify_all();
    }
    loop.loop();

    std::lock_guard<std::mutex> lg(_mtx);
    _loop = nullptr;
}

