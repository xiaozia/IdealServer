
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Channel.h
*   Last Modified : 2019-06-10 20:48
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_NET_CHANNEL_H
#define  _IDEAL_NET_CHANNEL_H

#include "ideal/base/NonCopyable.h"
#include "ideal/base/Timestamp.h"

#include <functional>
#include <memory>

namespace ideal {

namespace net {

class EventLoop;

class Channel : public ideal::NonCopyable {
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    void setReadCallback(ReadEventCallback cb) { _readCallback = std::move(cb); }
    void setWriteCallback(EventCallback cb) { _writeCallback = std::move(cb); }
    void setCloseCallback(EventCallback cb) { _closeCallback = std::move(cb); }
    void setErrorCallback(EventCallback cb) { _errorCallback = std::move(cb); }

    void handleEvent(Timestamp receiveTime);

    void tie(const std::shared_ptr<void>&);

    int fd() const { return _fd; }
    int events() const { return _events; }
    void setRevents(int revt) { _revents = revt; }
    bool isNoneEvent() const { return _events == kNoneEvent; }

    void enableReading() { _events |= kReadEvent; update(); }
    void disableReading() { _events &= ~kReadEvent; update(); }
    void enableWriting() { _events |= kWriteEvent; update(); }
    void disableWriting() { _events |= kWriteEvent; update(); }
    void disableAll() { _events = kNoneEvent; update(); }
    bool isWriting() const { return _events & kWriteEvent; } // 正在写
    bool isReading() const { return _events & kReadEvent; }  // 正在读

    int label() { return _label; }
    void setLabel(int label) { _label = label; }

    std::string reventsToString() const;
    std::string eventsToString() const;

    void doNotLogHup() { _logHup = false; }
    
    EventLoop* ownerLoop() { return _loop; }
    void remove();

private:
    void update();
    void handleEventWithGuard(Timestamp receiveTime);

    static std::string eventsToString(int fd, int ev);

private:
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;
   
    EventLoop* _loop;
    const int _fd;
    int _events;
    int _revents;
    int _label;
    bool _logHup;

    std::weak_ptr<void> _tie;
    bool _tied;

    bool _eventHandling;
    bool _addedToLoop;
    ReadEventCallback _readCallback;
    EventCallback _writeCallback;
    EventCallback _closeCallback;
    EventCallback _errorCallback;
};

}

}

#endif // _IDEAL_NET_CHANNEL_H

