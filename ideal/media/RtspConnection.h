
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : RtspConnection.h
*   Last Modified : 2019-12-02 19:23
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_MEDIA_RTSPCONNECTION_H
#define  _IDEAL_MEDIA_RTSPCONNECTION_H

#include "ideal/media/Media.h"
#include "ideal/net/Callbacks.h"

#include <memory>

namespace ideal {
namespace net {
class Buffer;
}
}

namespace ideal {
namespace media {

class Rtsp;
class RtspRequest;
class RtspResponse;
class RtpConnection;

class RtspConnection {
public:
    enum ConnectionMode {
        RTSP_SERVER,
        RTSP_PUSHER
    };

    enum ConnectionState {
        START_CONNECT,
        START_PLAY,
        START_PUSH
    };

    RtspConnection(Rtsp* rtsp, const ideal::net::TcpConnectionPtr& conn);
    ~RtspConnection();

    MediaSessionId getMediaSessionId() const { return _sessionId; }
    bool isPlay() const { return _connState == START_PLAY; }
    bool isRecord() const { return _connState == START_PUSH; }

    void handleRtspMessage(ideal::net::Buffer* buf);

private:
    void handleRtcp(ideal::net::Buffer* buf);
    void sendMessage(std::shared_ptr<char> buf, int size);
    
    bool handleRtspRequest(ideal::net::Buffer* buf);
    void handleRtspResponse(ideal::net::Buffer* buf);

    void handleCmdOption();
    void handleCmdDescribe();
    void handleCmdSetup();
    void handleCmdPlay();
    void handleCmdTeardown();
    void handleCmdGetParamter();

    void sendOptions(ConnectionMode mode = RTSP_SERVER);
    void sendDescribe();
    void sendAnnounce();
    void sendSetup();
    void handleRecord();


private:
    Rtsp* _rtsp = nullptr;
    const ideal::net::TcpConnectionPtr _tcpConn;
    ConnectionMode _connMode = RTSP_SERVER;
    ConnectionState _connState = START_CONNECT;
    MediaSessionId _sessionId = 0;

    std::shared_ptr<RtspRequest> _rtspRequestPtr;
    std::shared_ptr<RtspResponse> _rtspResponsePtr;
    std::shared_ptr<RtpConnection> _rtpConnPtr;
//    std::shared_ptr<Channel> _rtcpChannels[MAX_MEDIA_CHANNEL];
};

}
}

#endif // _IDEAL_MEDIA_RTSPCONNECTION_H

