
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : TcpClient.h
*   Last Modified : 2019-07-29 14:28
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_NET_TCPCLIENT_H
#define  _IDEAL_NET_TCPCLIENT_H

#include "ideal/base/NonCopyable.h"
#include "ideal/net/InetAddress.h"
#include "ideal/net/Callbacks.h"

#include <mutex>
#include <memory>

namespace ideal {

namespace net {

class EventLoop;
class Connector;
using ConnectorPtr = std::shared_ptr<Connector>;
class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

class TcpClient : public ideal::NonCopyable {
public:
    TcpClient(EventLoop* loop, const InetAddress& serverAddr, const std::string& name);
    ~TcpClient();

    void connect();
    void disconnect();
    void stop();

    const std::string& name() const { return _name; }
    bool retry() const { return _retry; }
    EventLoop* getLoop() const { return _loop; }

    TcpConnectionPtr connection() const {
        std::lock_guard<std::mutex> lg(_mtx);
        return _connection;
    }

    void setConnectionCallback(ConnectionCallback cb) { _connectionCallback = std::move(cb); }
    void setMessageCallback(MessageCallback cb) { _messageCallback = std::move(cb); }
    void setWriteCompleteCallback(WriteCompleteCallback cb) { _writeCompleteCallback = std::move(cb); }

private:
    void newConnection(int sockfd);
    void removeConnection(const TcpConnectionPtr& conn);

private:
    EventLoop* _loop;
    ConnectorPtr _connector;
    const std::string _name;
    
    ConnectionCallback _connectionCallback;
    MessageCallback _messageCallback;
    WriteCompleteCallback _writeCompleteCallback;

    bool _retry;
    bool _connect;
    int _nextConnId;
    mutable std::mutex _mtx;
    TcpConnectionPtr _connection;
};

}

}

#endif // _IDEAL_NET_TCPCLIENT_H

