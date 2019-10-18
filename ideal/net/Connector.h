
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Connector.h
*   Last Modified : 2019-07-05 22:21
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_NET_CONNECTOR_H
#define  _IDEAL_NET_CONNECTOR_H

#include "ideal/base/noncopyable.h"
#include "ideal/net/InetAddress.h"

#include <functional>
#include <memory>

namespace ideal {

namespace net {

class EventLoop;
class Channel;

class Connector : public ideal::noncopyable {
public:
    using NewConnectionCallback = std::function<void(int sockfd)>;

    Connector(EventLoop* loop, const InetAddress& serverAddr);
    ~Connector();

    void setNewConnectionCallback(const NewConnectionCallback& cb) { _newConnectionCallback = cb; }
    const InetAddress& serverAddress() const { return _serverAddr; }

    void start();
    void restart();
    void stop();
    
private:
    enum State {
        kDisconnected,
        kConnecting,
        kConnected
    };

private:
    void setState(State s) { _state = s; }
    void startInLoop();
    void stopInLoop();
    void connect();
    void connecting(int sockfd);
    void handleWrite();
    void handleError();
    void retry(int sockfd);
    int removeAndResetChannel();
    void resetChannel();

private:
    static const int kMaxRetryDelayMs = 30*1000;
    static const int kInitRetryDelayMs = 500;

    EventLoop* _loop;
    InetAddress _serverAddr;
    bool _connect;
    State _state;
    std::unique_ptr<Channel> _channel;
    NewConnectionCallback _newConnectionCallback;
    int _retryDelayMs;
};

}

}

#endif // _IDEAL_NET_CONNECTOR_H


