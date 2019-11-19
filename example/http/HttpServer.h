
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : HttpServer.h
*   Last Modified : 2019-11-19 20:58
*   Describe      :
*
*******************************************************/

#ifndef  _EXAMPLE_HTTP_HTTPSERVER_H
#define  _EXAMPLE_HTTP_HTTPSERVER_H

#include "ideal/net/TcpServer.h"
#include "ideal/net/EventLoop.h"

#include <functional>

class HttpRequest;
class HttpResponse;

class HttpServer {
public:
    using HttpCallback = std::function<void(const HttpRequest&, HttpResponse*)>;

    HttpServer(ideal::net::EventLoop* loop,
               const ideal::net::InetAddress& listenAddr,
               const std::string& name,
               ideal::net::TcpServer::Option option = ideal::net::TcpServer::kNoReusePort);

    ideal::net::EventLoop* getLoop() const { return _server.getLoop(); }

    void setHttpCallback(const HttpCallback& cb) { _httpCallback = cb; }
    void setThreadNum(int numThreads) { _server.setThreadNum(numThreads); }
    
    void start();

private:
    void onConnection(const ideal::net::TcpConnectionPtr& conn);
    void onMessage(const ideal::net::TcpConnectionPtr& conn, ideal::net::Buffer* buf, ideal::Timestamp receiveTime);
    void onRequest(const ideal::net::TcpConnectionPtr& conn, const HttpRequest& req);

private:
    ideal::net::TcpServer _server;
    HttpCallback _httpCallback;
};

#endif // _EXAMPLE_HTTP_HTTPSERVER_H


