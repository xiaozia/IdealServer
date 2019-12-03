
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Timer.h
*   Last Modified : 2019-11-23 14:11
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_NET_TIMER_H
#define  _IDEAL_NET_TIMER_H

#include "ideal/base/NonCopyable.h"
#include "ideal/base/Timestamp.h"
#include "ideal/net/Callbacks.h"

#include <atomic>
#include <stdint.h>

namespace ideal {

namespace net {

class Timer : ideal::NonCopyable {
public:
    Timer(TimerCallback cb, Timestamp when, double interval) :
        _callback(std::move(cb)),
        _expiration(when),
        _interval(interval),
        _repeat(interval > 0.0),
        _sequence(++s_numCreated) {
    }

    void run() const { _callback(); }
    Timestamp expiration() const { return _expiration; }
    bool IsRepeat() const { return _repeat; }
    int64_t sequence() const { return _sequence; }

    void restart(Timestamp now);
    static int64_t numCreated() { return s_numCreated; }

private:
    const TimerCallback _callback;
    Timestamp _expiration;
    const double _interval;
    const bool _repeat;
    const int64_t _sequence;
    
    static std::atomic<int64_t> s_numCreated;
};

}

}

#endif // _IDEAL_NET_TIMER_H


