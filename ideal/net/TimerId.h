
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : TimerId.h
*   Last Modified : 2019-06-10 19:33
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_NET_TIMERID_H
#define  _IDEAL_NET_TIMERID_H

#include "ideal/base/Copyable.h"

namespace ideal {

namespace net {

class Timer;

class TimerId : public ideal::Copyable {
public:
    TimerId() :
        _timer(nullptr),
        _sequence(0) {
    }

    TimerId(Timer* timer, int64_t seq) :
        _timer(timer),
        _sequence(seq) {
    }

    friend class TimerQueue;

private:
    Timer* _timer;
    int64_t _sequence;
};

}

}

#endif // _IDEAL_NET_TIMERID_H


