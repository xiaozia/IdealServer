
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Poller.h
*   Last Modified : 2019-06-11 16:39
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_NET_POLLER_H
#define  _IDEAL_NET_POLLER_H

#include "ideal/base/noncopyable.h"
#include "ideal/base/Timestamp.h"

#include <vector>
#include <map>

namespace ideal {

namespace net {

class Channel;
class EventLoop;

class Poller : public ideal::noncopyable {
public:
    using ChannelList = std::vector<Channel*>;

    Poller(EventLoop* loop);
    virtual ~Poller();

    virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) = 0;
    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;
    virtual bool hasChannel(Channel* channel) const;

    void assertInLoopThread() const;
    static Poller* newDefaultPoller(EventLoop* loop); // DefaultPoller.cc 

protected:
    using ChannelMap = std::map<int, Channel*>;
    ChannelMap _channels;

private:
    EventLoop* _loop;
};

}

}

#endif // _IDEAL_NET_POLLER_H

