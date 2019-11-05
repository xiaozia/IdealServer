
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Buffer.cc
*   Last Modified : 2019-11-05 16:01
*   Describe      :
*
*******************************************************/

#include "ideal/net/Buffer.h"
#include "ideal/net/SocketUtil.h"
#include "ideal/base/Types.h"

#include <errno.h>
#include <sys/uio.h>


using namespace ideal;
using namespace ideal::net;

const char Buffer::kCRLF[] = "\r\n";

ssize_t Buffer::readFd(int fd, int* savedErrno) {
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writable = writableBytes();
    
    vec[0].iov_base = begin() + _writerIndex;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;
    const int iovcnt = (writable < sizeof extrabuf)? 2 : 1;
    // readv()用于在一次函数调用中读多个非连续缓冲区
    const ssize_t n = sockets::readv(fd, vec, iovcnt);
    if(n < 0) {
        *savedErrno = errno;
    }
    else if(implicit_cast<size_t>(n) <= writable) {
        _writerIndex += n;
    }
    else {
        _writerIndex = _buffer.size();
        append(extrabuf, n - writable);
    }
    return n;
}
