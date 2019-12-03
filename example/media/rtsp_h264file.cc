
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : rtsp_h264file.cc
*   Last Modified : 2019-12-02 18:58
*   Describe      :
*
*******************************************************/

#include "H264File.h"
#include "ideal/net/EventLoop.h"
#include "ideal/media/H264Source.h"
#include "ideal/media/RtspServer.h"
#include "ideal/base/Logger.h"

#include <iostream>
#include <thread>
#include <chrono>

void sendFrameThread(ideal::media::RtspServer* rtspServer,
                     ideal::media::MediaSessionId sessionId, 
                     H264File* h264File) {
    LOG_INFO << "sendFrameThread start";
    size_t bufSize = 500000;
    uint8_t* frameBuf = new uint8_t[bufSize];

    while(1) {
        bool endOfFrame;
        int frameSize = h264File->readFrame((char*)frameBuf, bufSize, &endOfFrame);
        if(frameSize > 0) {
            ideal::media::AVFrame videoFrame = { 0 };
            videoFrame.size = frameSize;
            videoFrame.timestamp = ideal::media::H264Source::getTimestamp();
            videoFrame.buffer.reset(new uint8_t[videoFrame.size]);
            memcpy(videoFrame.buffer.get(), frameBuf, videoFrame.size);

            rtspServer->pushFrame(sessionId, ideal::media::channel_0, videoFrame);
        }
        else {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
    delete frameBuf;
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("Usage: %s test.264\n", argv[0]);
        return 0;
    }

    H264File h264File;
    if(!h264File.open(argv[1])) {
        printf("Open %s failed\n", argv[1]);
        return 0;
    }

    std::string ip = "0.0.0.0";
    ideal::net::InetAddress serverAddr(ip, 554);
    ideal::net::EventLoop loop;
    ideal::media::RtspServer server(&loop, serverAddr);

    ideal::media::MediaSession* session = ideal::media::MediaSession::creatNew("live");
    const std::string rtspUrl = "rtsp://" + ip + ":554/" + session->getRtspUrlSuffix();

    session->addMediaSource(ideal::media::channel_0, ideal::media::H264Source::createNew());
    ideal::media::MediaSessionId sessionId = server.addMediaSession(session);

    std::thread t(sendFrameThread, &server, sessionId, &h264File);
    t.detach();

    std::cout << "Play URL: " << rtspUrl << std::endl;

    server.start();
    loop.loop();
    return 0;
}

