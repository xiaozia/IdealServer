
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : H264Source.h
*   Last Modified : 2019-12-01 17:14
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_MEDIA_H264SOURCE_H
#define  _IDEAL_MEDIA_H264SOURCE_H

#include "ideal/media/MediaSource.h"

#include <string>

namespace ideal {
namespace media {

class H264Source : public MediaSource {
public:
    static H264Source* createNew(uint32_t frameRate = 25);  // 帧率25
    ~H264Source();

    void setFrameRate(uint32_t frameRate) {
        _frameRate = frameRate;
    }
    uint32_t getFrameRate() const { return _frameRate; }
    static uint32_t getTimestamp();

    virtual std::string getMediaDescription(uint16_t port);
    virtual std::string getAttribute();
    virtual bool handleFrame(MediaChannelId channelId, AVFrame frame);

private:
    H264Source(uint32_t frameRate);


private:
    uint32_t _frameRate;
};

}
}

#endif // _IDEAL_MEDIA_H264SOURCE_H

