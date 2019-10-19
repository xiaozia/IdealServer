
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Acceptor.h
*   Last Modified : 2019-07-05 15:51
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_NET_ACCEPTOR_H
#define  _IDEAL_NET_ACCEPTOR_H

#include "ideal/base/NonCopyable.h"
#include "ideal/net/Channel.h"
#include "ideal/net/Socket.h"

#include "functional"


namespace ideal {

namespace net {

class Acceptor : public ideal::NonCopyable {
public:
    using NewConnectionCallback = std::function<void(int sockfd, const InetAddress&)>;

    Acceptor(EventLoop* loop, const InetAddress& listenaddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback& cb) { _newConnectionCallback = cb; }
    bool listening() const { return _listening; }
    void listen();

private:
    void handleRead();

private:
    EventLoop* _loop;
    Socket _acceptSocket;
    Channel _acceptChannel;
    NewConnectionCallback _newConnectionCallback;
    bool _listening;
    int _idleFd;
};

}

}

#endif // _IDEAL_NET_ACCEPTOR_H

