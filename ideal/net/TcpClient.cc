
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : TcpClient.cc
*   Last Modified : 2019-12-01 13:25
*   Describe      :
*
*******************************************************/

#include "ideal/net/TcpClient.h"
#include "ideal/base/Logger.h"
#include "ideal/net/Connector.h"
#include "ideal/net/EventLoop.h"
#include "ideal/net/SocketUtil.h"
#include "ideal/net/TcpConnection.h"

namespace ideal {

namespace net {

void removeConnection(EventLoop* loop, const TcpConnectionPtr& conn) {
    loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

void removeConnector(const ConnectorPtr& conn) { }

}

}

using namespace ideal;
using namespace ideal::net;

TcpClient::TcpClient(EventLoop* loop, const InetAddress& serverAddr, const std::string& name) :
    _loop(loop),
    _connector(new Connector(loop, serverAddr)),
    _name(name),
    _connectionCallback(defaultConnectionCallback),
    _messageCallback(defaultMessageCallback),
    _retry(false),
    _connect(true),
    _nextConnId(1) {
    _connector->setNewConnectionCallback(
        std::bind(&TcpClient::newConnection, this, std::placeholders::_1));
    
    LOG_INFO << "TcpClient::TcpClient[" << _name
             << "] - connector " << _connector.get();
}

TcpClient::~TcpClient() {
    LOG_INFO << "TcpClient::~TcpClient[" << _name
             << "] - connector " << _connector.get();
    
    TcpConnectionPtr conn;
    bool unique = false;
    {
        std::lock_guard<std::mutex> lg(_mtx);
        unique = _connection.unique();
        conn = _connection;
    }
    if(conn) {
        assert(_loop == conn->getLoop());
        CloseCallback cb = std::bind(&ideal::net::removeConnection, _loop, std::placeholders::_1);
        _loop->runInLoop(std::bind(&TcpConnection::setCloseCallback, conn, cb));
        if(unique) {
            conn->forceClose();
        }
    }
    else {
        _connector->stop();
        _loop->runAfter(1, std::bind(&ideal::net::removeConnector, _connector));;
    }
}


void TcpClient::connect() {
    LOG_INFO << "TcpClient::connect[" << _name << "] - connecting to "
             << _connector->serverAddress().toIpPort();
    _connect = true;
    _connector->start();
}

void TcpClient::disconnect() {
    _connect = false;
    {
        std::lock_guard<std::mutex> lg(_mtx);
        if(_connection) {
            _connection->shutdown();
        }
    }
}

void TcpClient::stop() {
    _connect = false;
    _connector->stop();
}

void TcpClient::newConnection(int sockfd) {
    _loop->assertInLoopThread();
    InetAddress peerAddr(sockets::getPeerAddr(sockfd));
    char buf[32];
    snprintf(buf, sizeof buf, ":%s#%d", peerAddr.toIpPort().c_str(), _nextConnId);
    ++_nextConnId;
    std::string connName = _name + buf;

    InetAddress localAddr(sockets::getLocalAddr(sockfd));
    TcpConnectionPtr conn(new TcpConnection(_loop,
                                            connName,
                                            sockfd,
                                            localAddr,
                                            peerAddr));
    conn->setConnectionCallback(_connectionCallback);
    conn->setMessageCallback(_messageCallback);
    conn->setWriteCompleteCallback(_writeCompleteCallback);
    conn->setCloseCallback(std::bind(&TcpClient::removeConnection, this, std::placeholders::_1));
    {
        std::lock_guard<std::mutex> lg(_mtx);
        _connection = conn;
    }
    conn->connectEstablished();
}

void TcpClient::removeConnection(const TcpConnectionPtr& conn) {
    _loop->assertInLoopThread();
    assert(_loop == conn->getLoop());
    {
        std::lock_guard<std::mutex> lg(_mtx);
        assert(_connection == conn);
        _connection.reset();
    }

    _loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    if(_retry && _connect) {
        LOG_INFO << "TcpClient::connect[" << _name << "] - Reconnecting to "
                 << _connector->serverAddress().toIpPort();
        _connector->restart();
    }
}

