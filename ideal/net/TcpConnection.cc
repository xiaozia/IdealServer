
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : TcpConnection.cc
*   Last Modified : 2019-12-01 13:12
*   Describe      :
*
*******************************************************/

#include "ideal/net/TcpConnection.h"
#include "ideal/base/Logger.h"
#include "ideal/base/WeakCallback.h"
#include "ideal/net/Channel.h"
#include "ideal/net/EventLoop.h"
#include "ideal/net/Socket.h"
#include "ideal/net/SocketUtil.h"

#include <errno.h>

using namespace ideal;
using namespace ideal::net;

// used by TcpServer.cc 
void ideal::net::defaultConnectionCallback(const TcpConnectionPtr& conn) {
    LOG_TRACE << conn->peerAddress().toIpPort() << " -> "
              << conn->localAddress().toIpPort() << " is "
              << (conn->connected()? "UP" : "DOWN");
}

void ideal::net::defaultMessageCallback(const TcpConnectionPtr&, Buffer* buf, Timestamp) {
    buf->retrieveAll();  // discard
}

TcpConnection::TcpConnection(EventLoop* loop,
                             const std::string& name,
                             int sockfd,
                             const InetAddress& localAddr,
                             const InetAddress& peerAddr) :
    _loop(loop),
    _name(name),
    _state(kConnecting),
    _reading(true),
    _socket(new Socket(sockfd)),
    _channel(new Channel(loop, sockfd)),
    _localAddr(localAddr),
    _peerAddr(peerAddr),
    _highWaterMark(64*1024*1024) {
    _channel->setReadCallback(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
    _channel->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    _channel->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    _channel->setErrorCallback(std::bind(&TcpConnection::handleError, this));
    LOG_DEBUG << "TcpConnection::ctor[" << _name << "] at " << this 
              << " fd = " << sockfd;
    _socket->setKeepAlive(true);
}

TcpConnection::~TcpConnection() {
    LOG_DEBUG << "TcpConnection::dtor[" << _name << "] at " << this 
              << " fd = " << _channel->fd()
              << " state = " << stateToString();
    assert(_state == kDisconnected);
}

int TcpConnection::getFd() const { 
    return _channel->fd(); 
}

void TcpConnection::handleRead(Timestamp receiveTime) {
    _loop->assertInLoopThread();
    int savedErrno = 0;
    ssize_t n = _inputBuffer.readFd(_channel->fd(), &savedErrno);
    if(n > 0) {
        _messageCallback(shared_from_this(), &_inputBuffer, receiveTime);
    }
    else if(n == 0) {
        handleClose();
    }
    else {
        errno = savedErrno;
        LOG_SYSERR << "TcpConnection::handleRead";
        handleError();
    }
}

void TcpConnection::handleWrite() {
    _loop->assertInLoopThread();
    if(_channel->isWriting()) {
        ssize_t n = sockets::write(_channel->fd(), _outputBuffer.peek(), _outputBuffer.readableBytes());
        if(n > 0) {
            _outputBuffer.retrieve(n);
            if(_outputBuffer.readableBytes() == 0) {
                _channel->disableWriting();
                if(_writeCompleteCallback) {
                    _loop->queueInLoop(std::bind(_writeCompleteCallback, shared_from_this()));
                }
                if(_state == kDisconnecting) {
                    shutdownInLoop();
                }
            }
        }
        else {
            LOG_SYSERR << "TcpConenction::handleWrite";
        }
    }
    else {
        LOG_TRACE << "Connection fd = " << _channel->fd() << " is down, no more writing";
    }
}

void TcpConnection::handleClose() {
    _loop->assertInLoopThread();
    LOG_TRACE << "fd = " << _channel->fd() << " state = " << stateToString();
    assert(_state == kConnected || _state == kDisconnecting);

    setState(kDisconnected);
    _channel->disableAll();
    
    TcpConnectionPtr guardThis(shared_from_this());
    _connectionCallback(guardThis);
    _closeCallback(guardThis);
}

void TcpConnection::handleError() {
    int err = sockets::getSocketError(_channel->fd());
    LOG_ERROR << "TcpConnection::handleError [" << _name
              << "] - SO_ERROR = " << err << " " << strerror_tl(err);
}


void TcpConnection::send(const void* message, int len) {
    send(StringPiece(static_cast<const char*>(message), len));
}

void TcpConnection::send(const StringPiece& message) {
    if(_state == kConnected) {
        void (TcpConnection::*fp)(const StringPiece& message) = &TcpConnection::sendInLoop;
        _loop->runInLoop(std::bind(fp, this, message));
    }
}

void TcpConnection::send(Buffer* message) {
    if(_state == kConnected) {
        if(_loop->isInLoopThread()) {
            sendInLoop(message->peek(), message->readableBytes());
            message->retrieveAll();
        }
        else {
            void (TcpConnection::*fp)(const StringPiece& message) = &TcpConnection::sendInLoop;
            _loop->runInLoop(std::bind(fp, this, message->retrieveAllAsString()));
        }
    }
}

void TcpConnection::sendInLoop(const StringPiece& message) {
   sendInLoop(message.data(), message.size());
}

void TcpConnection::sendInLoop(const void* message, size_t len) {
    _loop->assertInLoopThread();
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool faultError = false;
    
    if(_state == kDisconnected) {
        LOG_WARN << "disconnected, give up writing";
        return;
    }

    if(!_channel->isWriting() && _outputBuffer.readableBytes() == 0) {
        nwrote = sockets::write(_channel->fd(), message, len);
        if(nwrote >= 0) {
            remaining = len - nwrote;
            if(remaining == 0 && _writeCompleteCallback) {
                _loop->queueInLoop(std::bind(_writeCompleteCallback, shared_from_this()));
            }
        }
        else {
            nwrote = 0;
            if(errno != EWOULDBLOCK) {
                LOG_SYSERR << "TcpConnection::sendInLoop";
                if(errno == EPIPE || errno == ECONNRESET) {
                    faultError = true;
                }
            }
        }
    }

    assert(remaining <= len);
    if(!faultError && remaining > 0) {
        size_t oldlen = _outputBuffer.readableBytes();
        if(oldlen + remaining >= _highWaterMark
           && oldlen < _highWaterMark && _highWaterMarkCallback) {
            _loop->queueInLoop(std::bind(_highWaterMarkCallback, shared_from_this(), oldlen + remaining));
        }
        _outputBuffer.append(static_cast<const char*>(message)+nwrote, remaining);
        
        if(!_channel->isWriting()) {
            _channel->enableWriting();
        }
    }
}

void TcpConnection::startRead() {
    _loop->runInLoop(std::bind(&TcpConnection::startReadInLoop, this));
}

void TcpConnection::startReadInLoop() {
    _loop->assertInLoopThread();
    if(!_reading || !_channel->isReading()) {
        _channel->enableReading();
        _reading = true;
    } 
}
   
void TcpConnection::stopRead() {
    _loop->runInLoop(std::bind(&TcpConnection::stopReadInLoop, this));
}

void TcpConnection::stopReadInLoop() {
    _loop->assertInLoopThread();
    if(_reading || _channel->isReading()) {
        _channel->disableReading();
        _reading = false;
    }
}

void TcpConnection::setTcpNoDelay(bool on) {
    _socket->setTcpNoDelay(on);
}

void TcpConnection::shutdown() {
    if(_state == kConnected) {
        setState(kDisconnecting);
        _loop->runInLoop(std::bind(&TcpConnection::shutdownInLoop, shared_from_this()));
    }
}

void TcpConnection::shutdownInLoop() {
    _loop->assertInLoopThread();
    if(!_channel->isWriting()) {
        _socket->shutdownWrite();
    }
}

void TcpConnection::forceClose() {
    if(_state == kConnected || _state == kDisconnecting) {
        setState(kDisconnecting);
        _loop->runInLoop(std::bind(&TcpConnection::forceCloseInLoop, shared_from_this()));
    }
}

void TcpConnection::forceCloseWithDelay(double seconds) {
    if(_state == kConnected || _state == kDisconnecting) {
        setState(kDisconnecting);
        _loop->runAfter(seconds, makeWeakCallback(shared_from_this(), &TcpConnection::forceClose));
    }
}

void TcpConnection::forceCloseInLoop() {
    _loop->assertInLoopThread();
    if(_state == kConnected || _state == kDisconnecting) {
        handleClose();
    }
}


const char* TcpConnection::stateToString() const {
    switch(_state) {
        case kDisconnected:
            return "kDisconnected";
        case kConnecting:
            return "kConnecting";
        case kConnected:
            return "kConnected";
        case kDisconnecting:
            return "kDisconnecting";
        default:
            return "unknown state";
    }
}

void TcpConnection::connectEstablished() {
    _loop->assertInLoopThread();

    assert(_state == kConnecting);
    setState(kConnected);

    _channel->tie(shared_from_this());
    _channel->enableReading();

    _connectionCallback(shared_from_this());
}

void TcpConnection::connectDestroyed() {
    _loop->assertInLoopThread();
    if(_state == kConnected) {
        setState(kDisconnected);
        _channel->disableAll();
        _connectionCallback(shared_from_this());
    }
    _channel->remove();

}

bool TcpConnection::getTcpInfo(struct tcp_info* ti) const {
    return _socket->getTcpInfo(ti);
}

std::string TcpConnection::getTcpInfoString() const {
    char buf[1024] = { 0 };
    _socket->getTcpInfoString(buf, sizeof buf);
    return buf;
}

