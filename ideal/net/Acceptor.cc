
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Acceptor.cc
*   Last Modified : 2019-07-05 21:50
*   Describe      :
*
*******************************************************/

#include "ideal/net/Acceptor.h"
#include "ideal/base/Logger.h"
#include "ideal/net/SocketUtil.h"
#include "ideal/net/EventLoop.h"
#include "ideal/net/InetAddress.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <assert.h>

using namespace ideal;
using namespace ideal::net;

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenaddr, bool reuseport) :
    _loop(loop),
    _acceptSocket(sockets::createNonblockingOrDie(listenaddr.family())),
    _acceptChannel(loop, _acceptSocket.fd()),
    _listening(false),
    _idleFd(::open("/dev/null", O_RDONLY | O_CLOEXEC)) {
    LOG_TRACE << "Acceptor::acceptor constructing";
    LOG_TRACE << "::open(\"/dev/null\") fd = " << _idleFd;
    assert(_idleFd >= 0);
    _acceptSocket.setReuseAddr(true);
    _acceptSocket.setReusePort(reuseport);
    _acceptSocket.bindAddress(listenaddr);
    _acceptChannel.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor() {
    LOG_TRACE << "Acceptor::~Acceptor destructing";
    _acceptChannel.disableAll();
    _acceptChannel.remove();
    ::close(_idleFd);
}

void Acceptor::listen() {
    _loop->assertInLoopThread();
    _listening = true;
    _acceptSocket.listen();
    _acceptChannel.enableReading();
}

void Acceptor::handleRead() {
    _loop->assertInLoopThread();
    InetAddress peerAddr;
    int connfd = _acceptSocket.accept(&peerAddr);
    if(connfd >= 0) {
        if(_newConnectionCallback) {
            _newConnectionCallback(connfd, peerAddr);
        }
        else {
            sockets::close(connfd);
        }
    }

    else {
        LOG_SYSERR << "Acceptor::handleRead";
        if(errno == EMFILE) {
            ::close(_idleFd);
            _idleFd = ::accept(_acceptSocket.fd(), NULL, NULL);
            ::close(_idleFd);
            _idleFd = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }
}

