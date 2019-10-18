
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : SocketUtil.cc
*   Last Modified : 2019-05-26 17:07
*   Describe      :
*
*******************************************************/

#include "ideal/net/SocketUtil.h"
#include "ideal/base/Logger.h"
#include "ideal/net/Endian.h"

#include <boost/implicit_cast.hpp>

#include <unistd.h>


namespace ideal {

namespace net {

const struct sockaddr_in* sockets::sockaddr_in_cast(const struct sockaddr* addr) {
    return static_cast<const struct sockaddr_in*>(boost::implicit_cast<const void*>(addr));
}

struct sockaddr* sockets::sockaddr_cast(struct sockaddr_in* addr) {
    return static_cast<struct sockaddr*>(boost::implicit_cast<void*>(addr));
}

const struct sockaddr* sockets::sockaddr_cast(const struct sockaddr_in* addr) {
    return static_cast<const struct sockaddr*>(boost::implicit_cast<const void*>(addr));
}


int sockets::createNonblockingOrDie(sa_family_t family) {
    int sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if(sockfd < 0) {
        LOG_SYSFATAL << "sockets::createNonblockingOrDie";
    }
    LOG_TRACE << "sockets::createNonblockingOrDie() fd = " << sockfd;
    return sockfd;
} 

void sockets::bindOrDie(int sockfd, const struct sockaddr* addr) {
    int ret = ::bind(sockfd, addr, static_cast<socklen_t>(sizeof *addr));
    if(ret < 0)
        LOG_SYSFATAL << "sockets::bindOrDie";
}

void sockets::listenOrDie(int sockfd) {
    int ret = ::listen(sockfd, SOMAXCONN);
    if(ret < 0)
        LOG_SYSFATAL << "sockets::listenOrDie";
}

int sockets::accept(int sockfd, struct sockaddr_in* addr) {
    socklen_t addrlen = static_cast<socklen_t>(sizeof *addr);
    int connfd = ::accept4(sockfd, sockaddr_cast(addr), &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if(connfd < 0) {
        int savedErrno = errno;
        switch(savedErrno) {
            case EAGAIN:
            case ECONNABORTED:
            case EINTR:
            case EPROTO:
            case EPERM:
            case EMFILE:
                LOG_SYSERR << "resolvable error of ::accept4" << savedErrno;
                errno = savedErrno;
                break;
            case EBADF:
            case EFAULT:
            case EINVAL:
            case ENOBUFS:
            case ENOMEM:
            case ENOTSOCK:
            case EOPNOTSUPP:
                LOG_FATAL << "unexpected error of ::accept4" << savedErrno;
                break;
            default:
                LOG_FATAL << "unknown error of ::accept4" << savedErrno;
                break;
        }
    }
    return connfd;
}

int sockets::connect(int sockfd, const struct sockaddr* addr) {
    return ::connect(sockfd, addr, static_cast<socklen_t>(sizeof *addr));
}

ssize_t sockets::read(int sockfd, void* buf, size_t count) {
    return ::read(sockfd, buf, count);
}

ssize_t sockets::readv(int sockfd, const struct iovec* iov, int iovcnt) {
    return ::readv(sockfd, iov, iovcnt);
}

ssize_t sockets::write(int sockfd, const void* buf, size_t count) {
    return ::write(sockfd, buf, count);
}

void sockets::close(int sockfd) {
    int ret = ::close(sockfd);
    if(ret < 0)
        LOG_SYSERR << "sockets::close";
}

void sockets::shutdownWrite(int sockfd) {
    int ret = ::shutdown(sockfd, SHUT_WR);
    if(ret < 0)
        LOG_SYSERR << "sockets::shutdownWrite";
}

void sockets::toIp(char* buf, size_t size, const struct sockaddr* addr) {
    assert(size >= INET_ADDRSTRLEN);
    const struct sockaddr_in* addr4 = sockaddr_in_cast(addr); 
    ::inet_ntop(AF_INET, &addr4->sin_addr, buf, static_cast<socklen_t>(size));
}

void sockets::toIpPort(char* buf, size_t size, const struct sockaddr* addr) {
    toIp(buf, size, addr);
    size_t end = ::strlen(buf);
    const struct sockaddr_in* addr4 = sockaddr_in_cast(addr);
    uint16_t port = sockets::networkToHost16(addr4->sin_port);
    assert(size > end);
    snprintf(buf + end, size - end, ":%u", port);

}

void sockets::fromIpPort(const char* ip, uint16_t port, struct sockaddr_in* addr) {
    addr->sin_family = AF_INET;
    addr->sin_port =hostToNetwork16(port);
    int ret = inet_pton(AF_INET, ip, &addr->sin_addr);
    if(ret < 0)
        LOG_SYSERR << "sockets::fromIpPort";
}

int sockets::getSocketError(int sockfd) {
    int optval;
    socklen_t optlen = static_cast<socklen_t>(sizeof optval);
    int ret = ::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen);
    if(ret < 0)
        return errno;
    else
        return optval;
}

struct sockaddr_in sockets::getLocalAddr(int sockfd) {
    struct sockaddr_in localaddr;
    memset(&localaddr, 0, sizeof localaddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
    int ret = ::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen);
    if(ret < 0) 
        LOG_SYSFATAL << "sockets::getLocalAddr";
    return localaddr;
}

struct sockaddr_in sockets::getPeerAddr(int sockfd) {
   struct sockaddr_in peeraddr;
   memset(&peeraddr, 0, sizeof peeraddr);
   socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
   int ret = ::getpeername(sockfd, sockaddr_cast(&peeraddr), &addrlen);
   if(ret < 0)
       LOG_SYSFATAL << "sockets::getPeerAddr";
   return peeraddr;
}

bool sockets::isSelfConnect(int sockfd) {
    struct sockaddr_in localaddr = getLocalAddr(sockfd);
    struct sockaddr_in peeraddr = getPeerAddr(sockfd);

    struct sockaddr_in* laddr = &localaddr;
    struct sockaddr_in* paddr = &peeraddr;

    return laddr->sin_port == paddr->sin_port &&
        laddr->sin_addr.s_addr == paddr->sin_addr.s_addr;
}

}

}

