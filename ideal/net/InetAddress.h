
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : InetAddress.h
*   Last Modified : 2019-06-03 19:48
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_NET_INETADDRESS_H
#define  _IDEAL_NET_INETADDRESS_H

#include "ideal/base/Copyable.h"
#include "ideal/base/StringPiece.h"

#include <netinet/in.h>

namespace ideal {

namespace net {

namespace sockets {
    const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);
}

class InetAddress : public ideal::Copyable {
public:
    explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false);
    InetAddress(StringArg ip, uint16_t port);
    explicit InetAddress(const struct sockaddr_in& addr) :
        _addr(addr) {
    }
    
    sa_family_t family() const { return _addr.sin_family; }
    std::string toIpPort() const;
    std::string toIp() const;
    uint16_t toPort() const;

    const struct sockaddr* getSockAddr() const {
        return sockets::sockaddr_cast(&_addr);
    }
    void setSockAddr(const struct sockaddr_in& addr) { _addr = addr; }

    uint32_t ipNetEndian() const{ return _addr.sin_addr.s_addr; }
    uint16_t portNetEndian() const { return _addr.sin_port; }

    static bool resolve(StringArg hostname, InetAddress* result);

private:
    struct sockaddr_in _addr;
};

}

}

#endif // _IDEAL_NET_INETADDRESS_H

