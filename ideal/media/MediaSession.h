
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : MediaSession.h
*   Last Modified : 2019-12-02 14:22
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_MEDIA_MEDIASESSION_H
#define  _IDEAL_MEDIA_MEDIASESSION_H

#include "ideal/media/Media.h"
#include "ideal/base/RingBuffer.h"

#include <memory>
#include <functional>
#include <string>
#include <vector>
#include <mutex>
#include <atomic>
#include <map>

namespace ideal {
namespace media {

class MediaSource;
class RtpConnection;

class MediaSession {
using NotifyCallback = std::function<void(MediaSessionId sessionId, uint32_t numClients)>;

public:
    static MediaSession* creatNew(const std::string& rtspUrlSuffix = "");
    ~MediaSession();

    MediaSessionId getMediaSessionId() { return _sessionId; }

    bool addMediaSource(MediaChannelId channelId, MediaSource* source);
    bool removeMediaSource(MediaChannelId channelId);

    std::string getRtspUrlSuffix() const { return _suffix; }
    void setRtspUrlSuffix(const std::string& suffix) {
        _suffix = suffix;
    }

    std::string getSdpMessage(const std::string& sessionName = "");
    MediaSource* getMediaSource(MediaChannelId channelId);

    bool addClient(int rtspfd, std::shared_ptr<RtpConnection> rtpConnPtr);
    void removeClient(int rtspfd);
    uint32_t getNumClient() const {
        return static_cast<uint32_t>(_clients.size());
    }

    bool handleFrame(MediaChannelId channelId, AVFrame frame);

    void setNotifyCallback(const NotifyCallback& cb) {
        _notifyCallback = cb;
    }

private:
    MediaSession(const std::string& rtspUrlSuffix);


private:
    static std::atomic_uint _lastMediaSessionId;

    MediaSessionId _sessionId = 0;
    std::string _suffix;
    std::string _sdp;
    
    std::mutex _clientMutex;
    std::mutex _mediaSourceMutex;
    std::vector<std::shared_ptr<MediaSource>> _mediaSources;
    std::map<int, std::weak_ptr<RtpConnection>> _clients;
    std::vector<ideal::RingBuffer<AVFrame>> _buffer;

    NotifyCallback _notifyCallback;
};

using MediaSessionPtr = std::shared_ptr<MediaSession>;

}
}

#endif // _IDEAL_MEDIA_MEDIASESSION_H

