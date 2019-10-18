
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : DefaultPoller.cc
*   Last Modified : 2019-06-11 20:05
*   Describe      :
*
*******************************************************/

#include "ideal/net/Poller.h"
#include "ideal/net/poller/EpollPoller.h"

using namespace ideal::net;

Poller* Poller::newDefaultPoller(EventLoop* loop) {
    return new EpollPoller(loop);
}

