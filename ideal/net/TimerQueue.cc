
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : TimerQueue.cc
*   Last Modified : 2019-06-11 21:10
*   Describe      :
*
*******************************************************/

#include "ideal/net/TimerQueue.h"

#include "ideal/base/Logger.h"
#include "ideal/net/Timer.h"
#include "ideal/net/TimerId.h"
#include "ideal/net/EventLoop.h"

#include <sys/timerfd.h>
#include <unistd.h>

// use in Channel_test.cc
namespace ideal {

namespace net {

int createTimerfd() {
    // CLOCK_MONOTONIC: nonsettable clock that is not affected 
    // by discontinuous changes in the system clock
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if(timerfd < 0) {
        LOG_SYSFATAL << "Failed in timerfd_create";
    }
    LOG_TRACE << "createTimerfd() fd = " << timerfd;
    return timerfd;
}

void readTimerfd(int timerfd, Timestamp now) {
    uint64_t howmany;
    ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
    LOG_TRACE << "TimerQueue::handleRead() " << howmany << " at " << now.toString();
    if(n != sizeof howmany) {
        LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
    }
}

}

}


using namespace ideal;
using namespace ideal::net;

struct timespec howmuchTimeFromNow(Timestamp when) {
    int64_t microseconds = when.microsecondsSinceEpoch() - Timestamp::now().microsecondsSinceEpoch();
    if(microseconds < 100) {
        microseconds = 100;
    }

    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicrosecondsPerSecond);
    ts.tv_nsec = static_cast<long>((microseconds % Timestamp::kMicrosecondsPerSecond) * 1000);

    return ts;
}

void resetTimerfd(int timerfd, Timestamp expiration) {
    struct itimerspec newValue;
    struct itimerspec oldValue;
    memset(&newValue, 0, sizeof newValue);
    memset(&oldValue, 0, sizeof oldValue);

    newValue.it_value = howmuchTimeFromNow(expiration);
    // arms (starts) or disarms (stops) the timer referred to by the file descriptor fd
    int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if(ret) {
        LOG_SYSERR << "timerfd_settime()";
    }
}

TimerQueue::TimerQueue(EventLoop* loop) :
    _loop(loop),
    _timerfd(createTimerfd()),
    _timerfdChannel(loop, _timerfd),
    _timers(),
    _callingExpiredTimers(false) {
    _timerfdChannel.setReadCallback(std::bind(&TimerQueue::handleRead, this));
    _timerfdChannel.enableReading();
}

TimerQueue::~TimerQueue() {
    _timerfdChannel.disableAll();
    _timerfdChannel.remove();
    ::close(_timerfd);
    for(const Entry& timer : _timers) {
        delete timer.second;
    }
}

TimerId TimerQueue::addTimer(TimerCallback cb, Timestamp when, double interval) {
    Timer* timer = new Timer(std::move(cb), when, interval);
    _loop->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
    return TimerId(timer, timer->sequence());
}

void TimerQueue::cancel(TimerId timerId) {
    _loop->runInLoop(std::bind(&TimerQueue::cancelInLoop, this, timerId));
}


void TimerQueue::addTimerInLoop(Timer* timer) {
    _loop->assertInLoopThread();
    bool earliestChanged = insert(timer);

    if(earliestChanged) {
        resetTimerfd(_timerfd, timer->expiration());
    }
}

void TimerQueue::cancelInLoop(TimerId timerId) {
    _loop->assertInLoopThread();
    assert(_timers.size() == _activeTimers.size());

    ActiveTimer timer(timerId._timer, timerId._sequence);
    ActiveTimerSet::iterator iter = _activeTimers.find(timer);
    if(iter != _activeTimers.end()) {
        size_t n = _timers.erase(Entry(iter->first->expiration(), iter->first));
        assert(n == 1);
        (void)n;
        delete iter->first;
        _activeTimers.erase(iter);
    }
    else if(_callingExpiredTimers) {
        _cancelingTimers.insert(timer);
    }

    assert(_timers.size() == _activeTimers.size());
}


void TimerQueue::handleRead() {
    _loop->assertInLoopThread();
    Timestamp now(Timestamp::now());
    readTimerfd(_timerfd, now);

    std::vector<Entry> expired = getExpired(now);

    _callingExpiredTimers = true;
    _cancelingTimers.clear();
    for(const Entry& iter : expired) {
        iter.second->run();
    }
    _callingExpiredTimers = false;

    reset(expired, now);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now) {
    assert(_timers.size() == _activeTimers.size());

    std::vector<Entry> expired;
    Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    TimerList::iterator end = _timers.upper_bound(sentry); // 大于
    assert(end == _timers.end() || now < end->first);

    std::copy(_timers.begin(), end, back_inserter(expired));
    _timers.erase(_timers.begin(), end);

    for(const Entry& iter : expired) {
        ActiveTimer timer(iter.second, iter.second->sequence());
        size_t n = _activeTimers.erase(timer);
        assert(n == 1);
        (void)n;
    }

    assert(_timers.size() == _activeTimers.size());
    return expired;
}

void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now) {
    Timestamp nextExpire;

    for(const Entry& iter : expired) {
        ActiveTimer timer(iter.second, iter.second->sequence());
        if(iter.second->IsRepeat() && _cancelingTimers.find(timer) == _cancelingTimers.end()) {
            iter.second->restart(now);
            insert(iter.second);
        }
        else {
            delete iter.second;
        }
    }
    if(!_timers.empty()) {
        nextExpire = _timers.begin()->second->expiration();
    }
    if(nextExpire.valid()) {
        resetTimerfd(_timerfd, nextExpire);
    }
}


bool TimerQueue::insert(Timer* timer) {
    _loop->assertInLoopThread();
    assert(_timers.size() == _activeTimers.size());
    bool earliestChanged = false;
    Timestamp when = timer->expiration();
    TimerList::iterator iter = _timers.begin();
    if(iter == _timers.end() || when < iter->first) { // 第一个或者到期时间更早
        earliestChanged = true;
    }
    {
        std::pair<TimerList::iterator, bool> result = _timers.insert(Entry(when, timer));
        assert(result.second);
        (void)result;
    }
    {
        std::pair<ActiveTimerSet::iterator, bool> result = _activeTimers.insert(ActiveTimer(timer, timer->sequence()));
        assert(result.second);
        (void)result;
    }
    assert(_timers.size() == _activeTimers.size());
    return earliestChanged;
}

