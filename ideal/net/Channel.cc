
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Channel.cc
*   Last Modified : 2019-06-11 16:28
*   Describe      :
*
*******************************************************/

#include "ideal/net/Channel.h"
#include "ideal/base/Logger.h"
#include "ideal/net/EventLoop.h"

#include <poll.h>
#include <sstream>

using namespace ideal;
using namespace ideal::net;

// /* Event types that can be polled for.  These bits may be set in `events'
//    to indicate the interesting event types; they will appear in `revents'
//    to indicate the status of the file descriptor.  */
// #define POLLIN		0x001		/* There is data to read.  */
// #define POLLPRI		0x002		/* There is urgent data to read.  */
// #define POLLOUT		0x004		/* Writing now will not block.  */
// 
// #if defined __USE_XOPEN || defined __USE_XOPEN2K8
// /* These values are defined in XPG4.2.  */
// # define POLLRDNORM	0x040		/* Normal data may be read.  */
// # define POLLRDBAND	0x080		/* Priority data may be read.  */
// # define POLLWRNORM	0x100		/* Writing now will not block.  */
// # define POLLWRBAND	0x200		/* Priority data may be written.  */
// #endif
// 
// #ifdef __USE_GNU
// /* These are extensions for Linux.  */
// # define POLLMSG	0x400
// # define POLLREMOVE	0x1000
// # define POLLRDHUP	0x2000
// #endif
// 
// /* Event types always implicitly polled for.  These bits need not be set in
//    `events', but they will appear in `revents' to indicate the status of
//    the file descriptor.  */
// #define POLLERR		0x008		/* Error condition.  */
// #define POLLHUP		0x010		/* Hung up.  */
// #define POLLNVAL	0x020		/* Invalid polling request.  */
const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd) :
    _loop(loop),
    _fd(fd),
    _events(0),
    _revents(0),
    _label(-1),
    _logHup(true),
    _tied(false),
    _eventHandling(false),
    _addedToLoop(false) {
}

Channel::~Channel() {
    assert(!_eventHandling);
    assert(!_addedToLoop);
    if(_loop->isInLoopThread()) {
        assert(!_loop->hasChannel(this));
    }
}

void Channel::update() {
    _addedToLoop = true;
    _loop->updateChannel(this);
}

void Channel::remove() {
    assert(isNoneEvent());
    _addedToLoop = false;
    _loop->removeChannel(this);
}

void Channel::tie(const std::shared_ptr<void>& obj) {
    _tie = obj;
    _tied = true;
}


void Channel::handleEvent(Timestamp receiveTime) {
    std::shared_ptr<void> guard;
    if(_tied) {
        guard = _tie.lock();
        if(guard) {
            handleEventWithGuard(receiveTime);
        }
    }
    else {
        handleEventWithGuard(receiveTime);
    }

}

void Channel::handleEventWithGuard(Timestamp receiveTime) {
    _eventHandling = true;
    LOG_TRACE << reventsToString();
    if((_revents & POLLHUP) && !(_revents & POLLIN)) {
        if(_logHup) {
            LOG_WARN << "fd = " << _fd << " Channel::handle_event() POLLHUP";
        }
        if(_closeCallback) {
            _closeCallback();
        }
    }
    
    if(_revents & POLLNVAL) {
        LOG_WARN << "fd = " << "Channel::handle_event() POLLNVAL";
    }

    if(_revents & (POLLERR | POLLNVAL)) {
        if(_errorCallback) {
            _errorCallback();
        }
    }

    if(_revents & (POLLIN | POLLPRI | POLLRDHUP)) {
        if(_readCallback) {
            _readCallback(receiveTime);
        }
    }

    if(_revents & POLLOUT) {
        if(_writeCallback) {
            _writeCallback();
        }
    }
    _eventHandling = false;
}

std::string Channel::reventsToString() const {
    return eventsToString(_fd, _revents);
}

std::string Channel::eventsToString() const {
    return eventsToString(_fd, _events);
}

std::string Channel::eventsToString(int fd, int ev) {
    std::ostringstream oss;
    oss << fd << ": ";
    if(ev & POLLIN)
        oss << "IN ";
    if(ev & POLLPRI)
        oss << "PRI ";
    if(ev & POLLOUT)
        oss << "OUT ";
    if(ev & POLLHUP)
        oss << "HUP ";
    if(ev & POLLRDHUP)
        oss << "RDHUP ";
    if(ev & POLLERR)
        oss << "ERR ";
    if(ev & POLLNVAL)
        oss << "NVAL ";

    return oss.str();
}



