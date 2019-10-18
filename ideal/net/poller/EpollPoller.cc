
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : EpollPoller.cc
*   Last Modified : 2019-06-11 17:13
*   Describe      :
*
*******************************************************/

#include "ideal/net/poller/EpollPoller.h"
#include "ideal/base/Logger.h"
#include "ideal/net/Channel.h"

#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>

using namespace ideal;
using namespace ideal::net;

// Channel的fd与epoll的联系
const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

EpollPoller::EpollPoller(EventLoop* loop) :
    Poller(loop), 
    _epollfd(::epoll_create1(EPOLL_CLOEXEC)),
    _events(kInitEventListSize) {
    if(_epollfd < 0) {
        LOG_SYSFATAL << "EpollPoller::EpollPoller";
    }
    LOG_TRACE << "epoll_create1() fd = " << _epollfd;
}

EpollPoller::~EpollPoller() {
    ::close(_epollfd);
}

Timestamp EpollPoller::poll(int timeoutMs, ChannelList* activeChannels) {
    LOG_TRACE << "fd total count " << _channels.size();
    int numEvents = ::epoll_wait(_epollfd, &*_events.begin(), static_cast<int>(_events.size()), timeoutMs);
    int savedErrno = errno;
    Timestamp now(Timestamp::now());
    if(numEvents > 0) {
        LOG_TRACE << numEvents << " events has happened";
        fillActiveChannels(numEvents, activeChannels);
        if(static_cast<size_t>(numEvents) == _events.size()) {
            _events.resize(_events.size() * 2);
        }
    }
    else if(numEvents == 0) {
        LOG_TRACE << "nothing happened";
    }
    else {
        if(savedErrno != EINTR) {
            errno = savedErrno;
            LOG_SYSERR << "EpollPoller::poll()";
        }
    }
    return now;
}

void EpollPoller::updateChannel(Channel* channel) {
    Poller::assertInLoopThread();
    const int label = channel->label();
    LOG_TRACE << "fd = " << channel->fd()
              << " events = " << channel->events()
              << " label = " << label;

    if(label == kNew || label == kDeleted) {
        int fd = channel->fd();
        if(label == kNew) {
            assert(_channels.find(fd) == _channels.end());
            _channels[fd] = channel;
        }
        else {
            assert(_channels.find(fd) != _channels.end());
            assert(_channels[fd] == channel);
        }

        channel->setLabel(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else {
        int fd = channel->fd();
        (void)fd;
        assert(_channels.find(fd) != _channels.end());
        assert(_channels[fd] == channel);
        assert(label == kAdded);
        if(channel->isNoneEvent()) { // 事件为空就删除
            update(EPOLL_CTL_DEL, channel);
            channel->setLabel(kDeleted);
        }
        else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EpollPoller::removeChannel(Channel* channel) {
    Poller::assertInLoopThread();
    int fd = channel->fd();
    LOG_TRACE << "fd = " << fd;
    assert(_channels.find(fd) != _channels.end());
    assert(_channels[fd] == channel);
    assert(channel->isNoneEvent());
    int label = channel->label();
    assert(label == kAdded || label == kDeleted);
    size_t n = _channels.erase(fd);
    (void)n;
    assert(n == 1);

    if(label == kAdded) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setLabel(kNew);
}

const char* EpollPoller::operationToString(int op) {
    switch(op) {
        case EPOLL_CTL_ADD:
            return "ADD";
        case EPOLL_CTL_DEL:
            return "DEL";
        case EPOLL_CTL_MOD:
            return "MOD";
        default:
            return "Unknown Operation";
    }
}

void EpollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const {
    assert(static_cast<size_t>(numEvents) <= _events.size()); // <=16
    for(int i=0; i<numEvents; ++i) {
        Channel* channel = static_cast<Channel*>(_events[i].data.ptr);
        channel->setRevents(_events[i].events);
        activeChannels->push_back(channel);
    }
}

void EpollPoller::update(int operation, Channel* channel) {
    struct epoll_event event;
    memset(&event, 0, sizeof event);
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    LOG_TRACE << "epoll_ctl op = " << operationToString(operation)
              << " fd = " << fd << " event = { " << channel->eventsToString() << " }";
    if(::epoll_ctl(_epollfd, operation, fd, &event) < 0) {
        if(operation == EPOLL_CTL_DEL) {
            LOG_SYSERR << "epoll_ctl op = " << operationToString(operation) << " fd = " << fd; 
        }
        else {
            LOG_SYSFATAL << "epoll_ctl op = " << operationToString(operation) << " fd = " << fd; 
        }
    }
}

