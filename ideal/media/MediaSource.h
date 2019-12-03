
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : MediaSource.h
*   Last Modified : 2019-12-02 12:48
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_MEDIA_MEDIASOURCE_H
#define  _IDEAL_MEDIA_MEDIASOURCE_H

#include "ideal/media/Media.h"
#include "ideal/media/Rtp.h"

#include <memory>
#include <string>
#include <functional>

namespace ideal {
namespace media {

class MediaSource {
using SendFrameCallback = std::function<bool(MediaChannelId channelId, RtpPacket pkt)>;

public:
    MediaSource() { }
    virtual ~MediaSource() { }

    MediaType getMediaType() const { return _mediaType; }
    uint32_t getPayloadType() const { return _payload; }
    uint32_t getClockRate() const { return _clockRate; }
   
    virtual std::string getMediaDescription(uint16_t port = 0) = 0;
    virtual std::string getAttribute() = 0;
    virtual bool handleFrame(MediaChannelId channelId, AVFrame frame) = 0;
    
    void setSendFrameCallback(const SendFrameCallback& cb) {
        _sendFrameCallback = cb;
    }

protected:
    // 继承
    MediaType _mediaType = NONE;
    uint32_t _payload = 0;
    uint32_t _clockRate = 0;
    SendFrameCallback _sendFrameCallback;
};

}
}

#endif // _IDEAL_MEDIA_MEDIASOURCE_H

