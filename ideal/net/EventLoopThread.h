
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : EventLoopThread.h
*   Last Modified : 2019-07-03 19:15
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_NET_EVENTLOOPTHREAD_H
#define  _IDEAL_NET_EVENTLOOPTHREAD_H

#include <ideal/base/Thread.h>

#include <mutex>
#include <condition_variable>

namespace ideal {

namespace net {

class EventLoop;

class EventLoopThread : public ideal::NonCopyable {
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;
 
    EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(), 
                    const std::string& name = std::string());
    ~EventLoopThread();
    EventLoop* startLoop();

private:
    void threadFunc();

private:
    EventLoop* _loop;
    bool _exiting;
    Thread _thread;
    std::mutex _mtx;
    std::condition_variable _cv;
    ThreadInitCallback _callback;
};

}

}

#endif // _IDEAL_NET_EVENTLOOPTHREAD_H

