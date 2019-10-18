
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Socket.h
*   Last Modified : 2019-05-25 09:34
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_NET_SOCKET_H
#define  _IDEAL_NET_SOCKET_H

#include "ideal/base/noncopyable.h"

struct tcp_info;

namespace ideal {

namespace net {

class InetAddress;

class Socket : public ideal::noncopyable {
public:
    explicit Socket(int sockfd) :
        _sockfd(sockfd) {
    }
    ~Socket();

    int fd() const { return _sockfd; }
    
    void bindAddress(const InetAddress& localaddr);
    void listen();
    int accept(InetAddress* peeraddr);
    void shutdownWrite();
    
    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);

    bool getTcpInfo(struct tcp_info*) const;
    bool getTcpInfoString(char* buf, int len) const;

private:
    const int _sockfd;
};

}

}

#endif // _IDEAL_NET_SOCKET_H

