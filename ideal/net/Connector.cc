
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Connector.cc
*   Last Modified : 2019-07-05 22:36
*   Describe      :
*
*******************************************************/

#include "ideal/net/Connector.h"
#include "ideal/base/Logger.h"
#include "ideal/net/Channel.h"
#include "ideal/net/EventLoop.h"
#include "ideal/net/SocketUtil.h"

#include <errno.h>
#include <assert.h>

using namespace ideal;
using namespace ideal::net;

const int Connector::kMaxRetryDelayMs;

Connector::Connector(EventLoop* loop, const InetAddress& serverAddr) :
    _loop(loop),
    _serverAddr(serverAddr),
    _connect(false),
    _state(kDisconnected),
    _retryDelayMs(kInitRetryDelayMs) {
}

Connector::~Connector() {
    assert(!_channel);
}

void Connector::start() {
    _connect = true;
    _loop->runInLoop(std::bind(&Connector::startInLoop, this));
}

void Connector::startInLoop() {
    _loop->assertInLoopThread();
    assert(_state == kDisconnected);
    if(_connect) {
        connect();
    }
    else {
        LOG_DEBUG << "do not connect";
    }
}

void Connector::connect() {
    int sockfd = sockets::createNonblockingOrDie(_serverAddr.family());
    int ret = sockets::connect(sockfd, _serverAddr.getSockAddr());
    int savedErrno = (ret == 0)? 0 : errno;
    switch(savedErrno) {
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
            connecting(sockfd);
            break;

        case EAGAIN:
        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case ECONNREFUSED:
        case ENETUNREACH:
            retry(sockfd);
            break;

        case EACCES:
        case EPERM:
        case EAFNOSUPPORT:
        case EALREADY:
        case EBADF:
        case EFAULT:
        case ENOTSOCK:
            LOG_SYSERR << "connect error in Connector::startInLoop " << savedErrno;
            sockets::close(sockfd);
            break;

        default:
            LOG_SYSERR << "Unexpected error in Connector::startInLoop " << savedErrno;
            sockets::close(sockfd);
            break;
    }
}

void Connector::connecting(int sockfd) {
    setState(kConnecting);
    assert(!_channel);
    _channel.reset(new Channel(_loop, sockfd));
    _channel->setWriteCallback(std::bind(&Connector::handleWrite, this));
    _channel->setErrorCallback(std::bind(&Connector::handleError, this));
    _channel->enableWriting();
}

void Connector::retry(int sockfd) {
    sockets::close(sockfd);
    setState(kDisconnected);
    if(_connect) {
        LOG_INFO << "Connector::retry - Retry connecting to " << _serverAddr.toIpPort()
                 << " in " << _retryDelayMs << " milliseconds. ";
        _loop->runAfter(_retryDelayMs/1000.0, std::bind(&Connector::startInLoop, this));
        _retryDelayMs = std::min(_retryDelayMs * 2, kMaxRetryDelayMs);
    }
    else {
        LOG_DEBUG << "do not connect";
    }
}

void Connector::handleWrite() {
    LOG_TRACE << "Conenctor::handleWrite state = " << _state;
    if(_state == kConnecting) {
        int sockfd = removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);
        if(err) {
            LOG_WARN << "Connector::handleWrite - SO_ERROR = " << err << " " << strerror_tl(err);
            retry(sockfd);
        }
        else if(sockets::isSelfConnect(sockfd)) {
            LOG_WARN << "Connector::handleWrite - Self Connect";
            retry(sockfd);
        }
        else {
            setState(kConnected);
            if(_connect) {
                _newConnectionCallback(sockfd);
            }
            else {
                sockets::close(sockfd);
            }
        }
    }
    else {
        assert(_state == kDisconnected);
    }
}

void Connector::handleError() {
    LOG_ERROR << "Conenctor::handleError state = " << _state;
    if(_state == kConnecting) {
        int sockfd = removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);
        LOG_TRACE << "Connector::handleError - SO_ERROR = " << err << " " << strerror_tl(err);
        retry(sockfd);
    }
}

int Connector::removeAndResetChannel() {
    _channel->disableAll();
    _channel->remove();
    int sockfd = _channel->fd();
    _loop->queueInLoop(std::bind(&Connector::resetChannel, this));
    return sockfd;
}

void Connector::resetChannel() {
    _channel.reset();
}

void Connector::restart() {
    _loop->assertInLoopThread();
    setState(kDisconnected);
    _retryDelayMs = kInitRetryDelayMs;
    _connect = true;
    startInLoop();
}

void Connector::stop() {
    _connect = false;
    _loop->queueInLoop(std::bind(&Connector::stopInLoop, this));
}

void Connector::stopInLoop() {
    _loop->assertInLoopThread();
    if(_state == kConnecting) {
        setState(kDisconnected);
        int sockfd = removeAndResetChannel();
        retry(sockfd);
    }
}

