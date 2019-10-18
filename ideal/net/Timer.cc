
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Timer.cc
*   Last Modified : 2019-06-10 20:18
*   Describe      :
*
*******************************************************/

#include "ideal/net/Timer.h"

using namespace ideal;
using namespace net;

std::atomic<int64_t> Timer::s_numCreated;

void Timer::restart(Timestamp now) {
    if(_repeat) {
        _expiration = addTime(now, _interval);
    }
    else {
        _expiration = Timestamp::invalid();
    }
}

