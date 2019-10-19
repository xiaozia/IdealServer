
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : EventLoop.h
*   Last Modified : 2019-06-22 14:37
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_NET_EVENTLOOP_H
#define  _IDEAL_NET_EVENTLOOP_H

#include "ideal/base/NonCopyable.h"
#include "ideal/base/Timestamp.h"
#include "ideal/base/CurrentThread.h"
#include "ideal/net/Callbacks.h"
#include "ideal/net/TimerId.h"

#include <atomic>
#include <mutex>
#include <vector>
#include <memory>

namespace ideal {

namespace net {

class Channel;
class Poller;
class TimerQueue;

class EventLoop : public ideal::NonCopyable {
public:
    using Functor = std::function<void()>;

    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    void runInLoop(Functor f);
    void queueInLoop(Functor f);
    size_t queueSize() const;

    TimerId runAt(Timestamp time, TimerCallback cb);
    TimerId runAfter(double delay, TimerCallback cb);
    TimerId runEvery(double interval, TimerCallback cb);
    void cancel(TimerId timerId);

    void wakeup();
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel);

    void assertInLoopThread() {
        if(!isInLoopThread()) {
            abortNotInLoopThread();
        }
    }

    bool isInLoopThread() const { return _threadId == CurrentThread::tid(); }

private:
    void abortNotInLoopThread();
    void handleRead();
    void doPendingFunctors();
    void printActiveChannels() const;

private:
    using ChannelList = std::vector<Channel*>;

    bool _looping;
    bool _eventHandling;
    bool _callingPendingFunctors;
    std::atomic<bool> _quit;

    int64_t _iteration;
    const pid_t _threadId;

    Timestamp _pollReturnTime;
    
    std::unique_ptr<Poller> _poller;
    std::unique_ptr<TimerQueue> _timerQueue;
    
    int _wakeupFd;
    std::unique_ptr<Channel> _wakeupChannel;

    ChannelList _activeChannels;
    Channel* _currentActiveChannel;

    mutable std::mutex _mtx;
    std::vector<Functor> _pendingFunctors;
};

}

}

#endif // _IDEAL_NET_EVENTLOOP_H


