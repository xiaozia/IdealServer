
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : TcpServer.h
*   Last Modified : 2019-11-25 14:54
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_NET_TCPSERVER_H
#define  _IDEAL_NET_TCPSERVER_H

#include "ideal/base/NonCopyable.h"
#include "ideal/net/TcpConnection.h"

#include <atomic>
#include <functional>
#include <map>

namespace ideal {

namespace net {

class Acceptor;
class EventLoop;
class EventLoopThreadPool;

class TcpServer : public ideal::NonCopyable {
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    enum Option {
        kNoReusePort,
        kReusePort
    };

    TcpServer(EventLoop* loop,
              const InetAddress& listenAddr,
              const std::string& name,
              Option option = kNoReusePort);
    ~TcpServer();

    const std::string ipPort() const { return _ipPort; }
    const std::string& name() const { return _name; }
    EventLoop* getLoop() const { return _loop; }
    std::shared_ptr<EventLoopThreadPool> getThreadPool() { return _threadPool; }
    
    void setThreadNum(int numThreads);
    void start();

    void setThreadInitCallback(const ThreadInitCallback& cb) { _threadInitCallback = cb; }
    void setConnectionCallback(const ConnectionCallback& cb) { _connectionCallback = cb; }
    void setMessageCallback(const MessageCallback& cb) { _messageCallback = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) { _writeCompleteCallback = cb; }

private:
    void newConnection(int sockfd, const InetAddress& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);

private:
    using ConnectionMap = std::map<std::string, TcpConnectionPtr>;

    EventLoop* _loop;
    const std::string _ipPort;
    const std::string _name;
    std::unique_ptr<Acceptor> _acceptor;
    std::shared_ptr<EventLoopThreadPool> _threadPool;

    ConnectionCallback _connectionCallback;
    MessageCallback _messageCallback;
    WriteCompleteCallback _writeCompleteCallback;
    ThreadInitCallback _threadInitCallback;

    std::atomic<bool> _started;
    int _nextConnId;
    ConnectionMap _connections;
};

}

}

#endif // _IDEAL_NET_TCPSERVER_H

