
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : EpollPoller.h
*   Last Modified : 2019-06-11 16:59
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_NET_POLLER_EPOLLPOLLER_H
#define  _IDEAL_NET_POLLER_EPOLLPOLLER_H

#include "ideal/net/Poller.h"

#include <vector>

struct epoll_event;

namespace ideal {

namespace net {

class EpollPoller : public ideal::net::Poller {
public:
    EpollPoller(EventLoop* loop);
    ~EpollPoller();

    Timestamp poll(int TimeoutMs, ChannelList* activeChannels) override;
    void updateChannel(Channel* channel) override;
    void removeChannel(Channel* channel) override;

private:
    static const int kInitEventListSize = 16;
    static const char* operationToString(int op);

    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
    void update(int operation, Channel* channel);

private:
    using EventList = std::vector<struct epoll_event>;
    
    int _epollfd;
    EventList _events;
};

}

}

#endif // _IDEAL_NET_POLLER_EPOLLPOLLER_H

