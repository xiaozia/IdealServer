
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : EventLoopThreadPool.cc
*   Last Modified : 2019-07-05 13:06
*   Describe      :
*
*******************************************************/

#include "ideal/net/EventLoopThreadPool.h"
#include "ideal/net/EventLoop.h"
#include "ideal/net/EventLoopThread.h"

#include <boost/implicit_cast.hpp>
#include <cassert>

using namespace ideal;
using namespace ideal::net;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, const std::string& name) :
    _baseLoop(baseLoop),
    _name(name),
    _started(false),
    _numThreads(0),
    _next(0) {
}

EventLoopThreadPool::~EventLoopThreadPool() {
}

void EventLoopThreadPool::start(const ThreadInitCallback& cb) {
    assert(!_started);
    _baseLoop->assertInLoopThread();
    _started = true;

    for(int i = 0; i<_numThreads; ++i) {
        char buf[_name.size() + 8];
        snprintf(buf, sizeof buf, "%s%d", _name.c_str(), i);
        EventLoopThread* t = new EventLoopThread(cb, buf);
        _threads.push_back(std::unique_ptr<EventLoopThread>(t));
        _loops.push_back(t->startLoop());
    }

    if(_numThreads == 0 && cb) {
        cb(_baseLoop);
    }
}

EventLoop* EventLoopThreadPool::getNextLoop() {
    _baseLoop->assertInLoopThread();
    assert(_started);
    EventLoop* loop = _baseLoop;

    if(!_loops.empty()) {
        // round-robin
        loop = _loops[_next++];
        if(boost::implicit_cast<size_t>(_next) == _loops.size()) {
            _next = 0;
        }
    }
    return loop;
}

EventLoop* EventLoopThreadPool::getLoopForHash(size_t hashCode) {
    _baseLoop->assertInLoopThread();
    assert(_started);
    EventLoop* loop = _baseLoop;

    if(!_loops.empty()) {
        loop = _loops[hashCode % _loops.size()];
    }
    return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops() {
    _baseLoop->assertInLoopThread();
    assert(_started);
    if(_loops.empty()) {
        return std::vector<EventLoop*>(1, _baseLoop);
    }
    else {
        return _loops;
    }
}

