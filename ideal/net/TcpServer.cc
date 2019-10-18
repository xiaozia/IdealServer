
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : TcpServer.cc
*   Last Modified : 2019-07-08 20:27
*   Describe      :
*
*******************************************************/

#include "ideal/net/TcpServer.h"
#include "ideal/base/Logger.h"
#include "ideal/net/Acceptor.h"
#include "ideal/net/EventLoop.h"
#include "ideal/net/EventLoopThreadPool.h"
#include "ideal/net/SocketUtil.h"

#include <stdio.h>

using namespace ideal;
using namespace ideal::net;

TcpServer::TcpServer(EventLoop* loop,
                     const InetAddress& listenAddr,
                     const std::string& name,
                     Option option) :
    _loop(loop),
    _ipPort(listenAddr.toIpPort()),
    _name(name),
    _acceptor(new Acceptor(loop, listenAddr, option == kReusePort)),
    _threadPool(new EventLoopThreadPool(loop, name)),
    _connectionCallback(defaultConnectionCallback),
    _messageCallback(defaultMessageCallback),
    _started(false),
    _nextConnId(1) {
    LOG_TRACE << "TcpServer::TcpServer [" << _name << "] constructing";
    _acceptor->setNewConnectionCallback(
            std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer() {
    _loop->assertInLoopThread();
    LOG_TRACE << "TcpServer::~TcpServer [" << _name << "] destructing";

    for(auto& item : _connections) {
        TcpConnectionPtr conn(item.second);
        item.second.reset();
        conn->getLoop()->runInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    }
}

void TcpServer::setThreadNum(int numThreads) {
    assert(numThreads >= 0);
    _threadPool->setThreadNum(numThreads);
}

void TcpServer::start() {
    LOG_TRACE << "TcpServer::start";
    if(!_started) {
        _started = true;
        _threadPool->start(_threadInitCallback);
        assert(!_acceptor->listening());
        _loop->runInLoop(
                std::bind(&Acceptor::listen, _acceptor.get()));
    }
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr) {
    _loop->assertInLoopThread();
    EventLoop* ioLoop = _threadPool->getNextLoop();
    char buf[64];
    snprintf(buf, sizeof buf, "-%s#%d", _ipPort.c_str(), _nextConnId);
    ++_nextConnId;
    std::string connName = _name + buf;

    LOG_INFO << "TcpServer::newConnection [" << _name
             << "] - new connection [" << connName
             << "] from " << peerAddr.toIpPort();

    InetAddress localAddr(sockets::getLocalAddr(sockfd));

    TcpConnectionPtr conn(new TcpConnection(ioLoop,
                                            connName,
                                            sockfd,
                                            localAddr,
                                            peerAddr));
    _connections[connName] = conn;
    conn->setConnectionCallback(_connectionCallback);
    conn->setMessageCallback(_messageCallback);
    conn->setWriteCompleteCallback(_writeCompleteCallback);
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, _1));
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn) {
    _loop->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn) {
    _loop->assertInLoopThread();
    LOG_INFO << "TcpServer::removeConnectionInLoop [" << _name
             << "] - connection " << conn->name();
    
    size_t n = _connections.erase(conn->name());
    assert(n == 1);
    (void)n;
    EventLoop* ioLoop = conn->getLoop();
    ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed,conn));
}

