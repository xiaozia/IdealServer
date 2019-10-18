
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Poller.cc
*   Last Modified : 2019-06-11 16:49
*   Describe      :
*
*******************************************************/

#include "ideal/net/Poller.h"
#include "ideal/net/EventLoop.h"
#include "ideal/net/Channel.h"

using namespace ideal;
using namespace ideal::net;

Poller::Poller(EventLoop* loop) :
    _loop(loop) {
}

Poller::~Poller() = default;

bool Poller::hasChannel(Channel* channel) const {
    assertInLoopThread();
    ChannelMap::const_iterator citer = _channels.find(channel->fd());
    return citer != _channels.end() && citer->second == channel;
}

void Poller::assertInLoopThread() const {
    _loop->assertInLoopThread();
}

