
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : EventLoopThreadPool.h
*   Last Modified : 2019-07-05 12:55
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_NET_EVENTLOOPTHREADPOOL_H
#define  _IDEAL_NET_EVENTLOOPTHREADPOOL_H

#include <ideal/base/noncopyable.h>

#include <memory>
#include <vector>
#include <string>

namespace ideal {

namespace net {

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : public ideal::noncopyable {
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    EventLoopThreadPool(EventLoop* baseLoop, const std::string& name);
    ~EventLoopThreadPool();

    void setThreadNum(int numThreads) { _numThreads = numThreads; }
    void start(const ThreadInitCallback& cb = ThreadInitCallback());

    EventLoop* getNextLoop();
    EventLoop* getLoopForHash(size_t hashCode);
    std::vector<EventLoop*> getAllLoops();

    bool started() const { return _started; }
    const std::string& name() const { return _name; }

private:
    EventLoop* _baseLoop;
    std::string _name;
    bool _started;
    int _numThreads;
    int _next;
    std::vector<std::unique_ptr<EventLoopThread>> _threads;
    std::vector<EventLoop*> _loops;
};

}

}

#endif // _IDEAL_NET_EVENTLOOPTHREADPOOL_H


