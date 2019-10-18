
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : EventLoop.cc
*   Last Modified : 2019-06-22 14:46
*   Describe      :
*
*******************************************************/

#include "ideal/net/EventLoop.h"
#include "ideal/base/Logger.h"
#include "ideal/net/Channel.h"
#include "ideal/net/Poller.h"
#include "ideal/net/TimerQueue.h"
#include "ideal/net/SocketUtil.h"

#include <algorithm>
#include <sys/eventfd.h>
#include <unistd.h>

using namespace ideal;
using namespace ideal::net;

__thread EventLoop* t_loopInThisThread = nullptr;

const int kPollTimeMs = 10000;

int createEventfd() {
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(evtfd < 0) {
        LOG_SYSFATAL << "Failed in eventfd";
    }
    LOG_TRACE << "createEventfd() fd = " << evtfd;
    return evtfd;
}

EventLoop::EventLoop() :
    _looping(false),
    _eventHandling(false),
    _callingPendingFunctors(false),
    _quit(false),
    _iteration(0),
    _threadId(CurrentThread::tid()),
    _poller(Poller::newDefaultPoller(this)),
    _timerQueue(new TimerQueue(this)),
    _wakeupFd(createEventfd()),
    _wakeupChannel(new Channel(this, _wakeupFd)),
    _currentActiveChannel(nullptr) {
    LOG_DEBUG << "EventLoop created " << this << " in thread " << _threadId;
    if(t_loopInThisThread) {
        LOG_FATAL << "Another EventLoop " << t_loopInThisThread
                  << " exists in this thread " << _threadId;
    }
    else {
        t_loopInThisThread = this;
    }

    _wakeupChannel->setReadCallback(std::bind(&EventLoop::handleRead, this));
    _wakeupChannel->enableReading();
}

EventLoop::~EventLoop() {
    LOG_DEBUG << "EventLoop " << this << " of thread " << _threadId
              << " destructs in thread " << CurrentThread::tid();
    _wakeupChannel->disableAll();
    _wakeupChannel->remove();
    ::close(_wakeupFd);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop() {
    assert(!_looping);
    assertInLoopThread();
    _looping = true;
    _quit = false;
    LOG_TRACE << "EventLoop " << this << " start looping";

    while(!_quit) {
        _activeChannels.clear();
        _pollReturnTime = _poller->poll(kPollTimeMs, &_activeChannels);
        ++_iteration;

        if(Logger::logLevel() <= Logger::TRACE) {
            printActiveChannels();
        }

        _eventHandling = true;
        for(Channel* channel : _activeChannels) {
            _currentActiveChannel = channel;
            _currentActiveChannel->handleEvent(_pollReturnTime);
        }
        _currentActiveChannel = nullptr;
        _eventHandling = false;

        doPendingFunctors();
    }

    LOG_TRACE << "EventLoop " << this << " stop looping";
    _looping = false;
}

void EventLoop::quit() {
    _quit = true;
    if(!isInLoopThread()) {
        wakeup();
    }
}

void EventLoop::runInLoop(Functor cb) {
    if(isInLoopThread()) {
        cb();
    }
    else {
        queueInLoop(std::move(cb));
    }
}

void EventLoop::queueInLoop(Functor cb) {
    {
        std::lock_guard<std::mutex> lg(_mtx);
        _pendingFunctors.push_back(std::move(cb));
    }

    if(!isInLoopThread() || _callingPendingFunctors) {
        wakeup();
    }
}

size_t EventLoop::queueSize() const {
    std::lock_guard<std::mutex> lg(_mtx);
    return _pendingFunctors.size();
}

TimerId EventLoop::runAt(Timestamp time, TimerCallback cb) {
    return _timerQueue->addTimer(cb, time, 0.0);
}

TimerId EventLoop::runAfter(double delay, TimerCallback cb) {
    Timestamp time(addTime(Timestamp::now(), delay));
    return runAt(time, std::move(cb));
}

TimerId EventLoop::runEvery(double interval, TimerCallback cb) {
    Timestamp time(addTime(Timestamp::now(), interval));
    return _timerQueue->addTimer(std::move(cb), time, interval);
}

void EventLoop::cancel(TimerId timerId) {
    return _timerQueue->cancel(timerId);
}

void EventLoop::updateChannel(Channel* channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    _poller->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    if(_eventHandling) {
    	assert(_currentActiveChannel == channel ||
        std::find(_activeChannels.begin(), _activeChannels.end(), channel) == _activeChannels.end());
  	}
  	_poller->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    return _poller->hasChannel(channel);
}

void EventLoop::handleRead() {
    uint64_t one;
    ssize_t n = sockets::read(_wakeupFd, &one, sizeof one);
    if(n != sizeof one) {
        LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
    }
}

void EventLoop::abortNotInLoopThread() {
    LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
              << " was created in threadId = " << _threadId
              << ", current thread id = " << CurrentThread::tid();
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = sockets::write(_wakeupFd, &one, sizeof one);
    if(n != sizeof one) {
        LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
    }
}

void EventLoop::printActiveChannels() const {
    for(const Channel* channel : _activeChannels) {
        LOG_TRACE << "{ " << channel->reventsToString() << " }";
    }
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    _callingPendingFunctors = true;
    {
        std::lock_guard<std::mutex> lg(_mtx);
        functors.swap(_pendingFunctors);
    }

    for(const Functor& functor : functors) {
        functor();
    }

    _callingPendingFunctors = false;
}

