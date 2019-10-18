
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : SocketUtil.h
*   Last Modified : 2019-05-24 21:26
*   Describe      :
*
*******************************************************/

#ifndef  _NET_SOCKETUTIL_H
#define  _NET_SOCKETUTIL_H


#include <arpa/inet.h>

namespace ideal {

namespace net {

namespace sockets {

const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr);
struct sockaddr* sockaddr_cast(struct sockaddr_in* addr);  // overload
const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);

int createNonblockingOrDie(sa_family_t family);
void bindOrDie(int sockfd, const struct sockaddr* addr);
void listenOrDie(int sockfd);
int accept(int sockfd, struct sockaddr_in* addr);
int connect(int sockfd, const struct sockaddr* addr);
ssize_t read(int sockfd, void* buf, size_t count);
ssize_t readv(int sockfd, const struct iovec* iov, int iovcnt);
ssize_t write(int sockfd, const void* buf, size_t count);
void close(int sockfd);
void shutdownWrite(int sockfd);

void toIpPort(char* buf, size_t size, const struct sockaddr* addr);
void toIp(char* buf, size_t size, const struct sockaddr* addr);
void fromIpPort(const char* ip, uint16_t port, struct sockaddr_in* addr);

int getSocketError(int sockfd);
struct sockaddr_in getLocalAddr(int sockfd);
struct sockaddr_in getPeerAddr(int sockfd);
bool isSelfConnect(int sockfd);

}

}

}


#endif // _NET_SOCKETUTIL_H


