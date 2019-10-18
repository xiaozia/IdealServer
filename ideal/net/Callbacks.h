
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Callbacks.h
*   Last Modified : 2019-06-10 19:16
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_NET_CALLBACKS_H
#define  _IDEAL_NET_CALLBACKS_H

#include "ideal/base/Timestamp.h"

#include <memory>
#include <functional>

namespace ideal {

namespace net {

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

using TimerCallback = std::function<void()>;

class Buffer;
class TcpConnection;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using CloseCallback = std::function<void(const TcpConnectionPtr&)>;
using WriteCompleteCallback = std::function<void(const TcpConnectionPtr&)>;
using HighWaterMarkCallback = std::function<void(const TcpConnectionPtr&, size_t)>;
using MessageCallback = std::function<void(const TcpConnectionPtr&, Buffer*, Timestamp)>;

// used by TcpServer.cc
void defaultConnectionCallback(const TcpConnectionPtr& conn);
void defaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp receiveTime);

}

}

#endif // _IDEAL_NET_CALLBACKS_H


