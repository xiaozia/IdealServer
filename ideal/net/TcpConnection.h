
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : TcpConnection.h
*   Last Modified : 2019-07-06 14:39
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_NET_TCPCONNECTION_H
#define  _IDEAL_NET_TCPCONNECTION_H

#include "ideal/base/NonCopyable.h"
#include "ideal/base/StringPiece.h"
#include "ideal/net/Buffer.h"
#include "ideal/net/InetAddress.h"
#include "ideal/net/Callbacks.h"

#include <memory>
#include <string>
#include <boost/any.hpp>

struct tcp_info;

namespace ideal {

namespace net {

class Channel;
class EventLoop;
class Socket;

class TcpConnection : public ideal::NonCopyable,
                      public std::enable_shared_from_this<TcpConnection> {
public:
    TcpConnection(EventLoop* loop, 
                  const std::string& name,
                  int sockfd,
                  const InetAddress& localAddr,
                  const InetAddress& peerAddr);
    ~TcpConnection();

    EventLoop* getLoop() const { return _loop; }
    const std::string name() const { return _name; }
    const InetAddress& localAddress() const { return _localAddr; }
    const InetAddress& peerAddress() const { return _peerAddr; }
    bool connected() const { return _state == kConnected; }
    bool disconnected() const { return _state == kDisconnected; }

    bool getTcpInfo(struct tcp_info*) const;
    std::string getTcpInfoString() const;

    void send(const void* message, int len);
    void send(const StringPiece& message);
    void send(Buffer* message);
    void shutdown();
    void forceClose();
    void forceCloseWithDelay(double seconds);
    void setTcpNoDelay(bool on);

    void startRead();
    void stopRead();
    bool isReading() const { return _reading; }

    void setContext(const boost::any& context) { _context = context; }
    const boost::any& getContext() const { return _context; }
    boost::any* getMutableContext() { return &_context; }

    void setConnectionCallback(const ConnectionCallback& cb) { _connectionCallback = cb; }
    void setMessageCallback(const MessageCallback& cb) { _messageCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) { _writeCompleteCallback = cb; }
    void setHighWaterMarkCallback(const HighWaterMarkCallback& cb, size_t highWaterMark) {
        _highWaterMarkCallback = cb;
        _highWaterMark = highWaterMark;
    }
    void setCloseCallback(const CloseCallback& cb) { _closeCallback = cb; }

    Buffer* inputBuffer() { return &_inputBuffer; }
    Buffer* outputBuffer() { return &_outputBuffer; }

    void connectEstablished();
    void connectDestroyed();

private:
    enum State {
        kDisconnected,
        kConnecting,
        kConnected,
        kDisconnecting
    };

private:
    void setState(State s) { _state = s; }
    const char* stateToString() const;
    void startReadInLoop();
    void stopReadInLoop();

    void handleRead(Timestamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();

    void sendInLoop(const StringPiece& message);
    void sendInLoop(const void* message, size_t len);
    void shutdownInLoop();
    void forceCloseInLoop();

private:
    EventLoop* _loop;
    const std::string _name;
    State _state;
    bool _reading;
    std::unique_ptr<Socket> _socket;
    std::unique_ptr<Channel> _channel;
    const InetAddress _localAddr;
    const InetAddress _peerAddr;
    
    ConnectionCallback _connectionCallback;
    MessageCallback _messageCallback;
    WriteCompleteCallback _writeCompleteCallback;
    HighWaterMarkCallback _highWaterMarkCallback;
    CloseCallback _closeCallback;

    size_t _highWaterMark;
    Buffer _inputBuffer;
    Buffer _outputBuffer;
    boost::any _context;
};

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

}

}

#endif // _IDEAL_NET_TCPCONNECTION_H

