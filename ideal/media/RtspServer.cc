
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : RtspServer.cc
*   Last Modified : 2019-12-02 19:50
*   Describe      :
*
*******************************************************/

#include "ideal/media/RtspServer.h"
#include "ideal/media/RtspConnection.h"
#include "ideal/net/EventLoop.h"
#include "ideal/base/Logger.h"

#include <memory>

using namespace ideal::media;

RtspServer::RtspServer(ideal::net::EventLoop* loop, const ideal::net::InetAddress& listenAddr) :
    _server(loop, listenAddr, "RtspServer") {
    _server.setConnectionCallback(
        std::bind(&RtspServer::onConnection, this, std::placeholders::_1));
    _server.setMessageCallback(
        std::bind(&RtspServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }

RtspServer::~RtspServer() { }

void RtspServer::start() {
    _server.start();
}

void RtspServer::onConnection(const ideal::net::TcpConnectionPtr& conn) {
    LOG_INFO << conn->peerAddress().toIpPort() << " -> "
             << conn->localAddress().toIpPort() << " is "
             << (conn->connected()? "UP" : "DOWN");

    if(conn->connected()) {
        // rtsp连接
	    _rtspConnMap[conn->name()] = std::make_shared<RtspConnection>(this, conn);
    }
    else {
        _rtspConnMap.erase(conn->name());
    }
}

void RtspServer::onMessage(const ideal::net::TcpConnectionPtr conn, ideal::net::Buffer* buf, ideal::Timestamp) {
    _rtspConnMap[conn->name()]->handleRtspMessage(buf);
}

MediaSessionId RtspServer::addMediaSession(MediaSession* session) {
    LOG_INFO << "RtspServer::addMediaSession";
    std::lock_guard<std::mutex> lg(_sessionMutex);
    if(_rtspSuffixMap.find(session->getRtspUrlSuffix()) != _rtspSuffixMap.end())
        return 0;

    std::shared_ptr<MediaSession> mediaSession(session);
    MediaSessionId sessionId = mediaSession->getMediaSessionId();
    LOG_INFO << sessionId << "," << _rtspSuffixMap.size() << ", " << _mediaSessions.size();
    _rtspSuffixMap.emplace(mediaSession->getRtspUrlSuffix(), sessionId);
    _mediaSessions.emplace(sessionId, std::move(mediaSession));
    LOG_INFO << _rtspSuffixMap.size() << ", " << _mediaSessions.size();
 
    return sessionId;
}

void RtspServer::removeMediaSession(MediaSessionId sessionId) {
    std::lock_guard<std::mutex> lg(_sessionMutex);
    auto iter = _mediaSessions.find(sessionId);
    if(iter != _mediaSessions.end()) {
        _rtspSuffixMap.erase(iter->second->getRtspUrlSuffix());
        _mediaSessions.erase(sessionId);
    }
}

bool RtspServer::pushFrame(MediaSessionId sessionId, MediaChannelId channelId, AVFrame frame) {
    std::shared_ptr<MediaSession> sessionPtr = nullptr;

    {
        std::lock_guard<std::mutex> lg(_sessionMutex);
        auto iter = _mediaSessions.find(sessionId);
        if(iter != _mediaSessions.end()) {
            sessionPtr = iter->second;
        }
        else {
            return false;
        }
    }

    if(sessionPtr != nullptr && sessionPtr->getNumClient() != 0) {
        return sessionPtr->handleFrame(channelId, frame);
    }
    
    return false;
}


MediaSessionPtr RtspServer::lookMediaSession(const std::string& suffix) {
    LOG_INFO << "RtspServer::lookMediaSession";
    std::lock_guard<std::mutex> lg(_sessionMutex);
    auto iter = _rtspSuffixMap.find(suffix);
    if(iter != _rtspSuffixMap.end()) {
        MediaSessionId id = iter->second;
        LOG_INFO << id;
        return _mediaSessions[id];
    }
    return nullptr;
}

MediaSessionPtr RtspServer::lookMediaSession(MediaSessionId sessionId) {
    std::lock_guard<std::mutex> lg(_sessionMutex);
    auto iter = _mediaSessions.find(sessionId);
    if(iter != _mediaSessions.end()) {
        return iter->second;
    }
    return nullptr;
}

