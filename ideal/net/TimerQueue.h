
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : TimerQueue.h
*   Last Modified : 2019-06-11 20:59
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_NET_TIMERQUEUE_H
#define  _IDEAL_NET_TIMERQUEUE_H

#include "ideal/base/Timestamp.h"
#include "ideal/net/Callbacks.h"
#include "ideal/net/Channel.h"

#include <utility>
#include <memory>
#include <vector>
#include <set>

namespace ideal {

namespace net {

class EventLoop;
class Timer;
class TimerId;

class TimerQueue : public ideal::noncopyable {
public:
    explicit TimerQueue(EventLoop* loop);
    ~TimerQueue();

    TimerId addTimer(TimerCallback cb, Timestamp when, double interval);
    void cancel(TimerId timerId);

private:
    using Entry = std::pair<Timestamp, Timer*>;
    using TimerList = std::set<Entry>;
    using ActiveTimer = std::pair<Timer*, int64_t>;
    using ActiveTimerSet = std::set<ActiveTimer>;

    void addTimerInLoop(Timer* timer);
    void cancelInLoop(TimerId timerId);
    void handleRead();

    std::vector<Entry> getExpired(Timestamp now);
    void reset(const std::vector<Entry>& expired, Timestamp now);
    bool insert(Timer* timer);

private:
    EventLoop* _loop;
    const int _timerfd;
    Channel _timerfdChannel;
    TimerList _timers;

    ActiveTimerSet _activeTimers;
    bool _callingExpiredTimers;
    ActiveTimerSet _cancelingTimers;
};

}

}

#endif // _IDEAL_NET_TIMERQUEUE_H


