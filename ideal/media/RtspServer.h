
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : RtspServer.h
*   Last Modified : 2019-12-02 19:50
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_MEDIA_RTSPSERVER_H
#define  _IDEAL_MEDIA_RTSPSERVER_H

#include "ideal/media/Media.h"
#include "ideal/media/Rtsp.h"
#include "ideal/net/TcpServer.h"

#include <memory>
#include <string>
#include <mutex>
#include <unordered_map>

namespace ideal {
namespace media {

class RtspConnection;

class RtspServer : public Rtsp {
public:
    RtspServer(ideal::net::EventLoop* loop, const ideal::net::InetAddress& addr);
    ~RtspServer();

    void start();
    void onConnection(const ideal::net::TcpConnectionPtr& conn);
    void onMessage(const ideal::net::TcpConnectionPtr conn, ideal::net::Buffer* buf, ideal::Timestamp);
   
    MediaSessionId addMediaSession(MediaSession* session);
    void removeMediaSession(MediaSessionId sessionId);

    bool pushFrame(MediaSessionId sessionId, MediaChannelId channelId, AVFrame frame);
 
private:
    virtual MediaSessionPtr lookMediaSession(const std::string& suffix);
    virtual MediaSessionPtr lookMediaSession(MediaSessionId sessionId);

private:
    using RtspConnectionMap = std::unordered_map<std::string, std::shared_ptr<RtspConnection>>;

    ideal::net::TcpServer _server;
    RtspConnectionMap _rtspConnMap;

    std::mutex _sessionMutex;
    std::unordered_map<MediaSessionId, std::shared_ptr<MediaSession>> _mediaSessions;
    std::unordered_map<std::string, MediaSessionId> _rtspSuffixMap;
};

}
}

#endif // _IDEAL_MEDIA_RTSPSERVER_H

