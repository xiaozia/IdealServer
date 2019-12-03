
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : HttpServer.cc
*   Last Modified : 2019-12-01 13:20
*   Describe      :
*
*******************************************************/

#include "HttpServer.h"
#include "HttpContext.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

#include "ideal/base/Any.h"
#include "ideal/base/Logger.h"

void defaultHttpCallback(const HttpRequest&, HttpResponse* resp) {
    resp->setStatusCode(HttpResponse::k404NotFound);
    resp->setStatusMessage("Not Found");
    resp->setCloseConnection(true);
}

HttpServer::HttpServer(ideal::net::EventLoop* loop,
                       const ideal::net::InetAddress& listenAddr,
                       const std::string& name,
                       ideal::net::TcpServer::Option option) :
    _server(loop, listenAddr, name, option),
    _httpCallback(defaultHttpCallback) {
    _server.setConnectionCallback(
        std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
    _server.setMessageCallback(
        std::bind(&HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void HttpServer::start() {
    LOG_WARN << "HttpServer[" << _server.name()
             << "] starts listenning on " << _server.ipPort();
    _server.start();
}
 
void HttpServer::onConnection(const ideal::net::TcpConnectionPtr& conn) {
    if(conn->connected()) {
        conn->setContext(HttpContext());
    }
}

void HttpServer::onMessage(const ideal::net::TcpConnectionPtr& conn,
                           ideal::net::Buffer* buf,
                           ideal::Timestamp receiveTime) {
    HttpContext* context = &(*conn->getMutableContext()).any_cast<HttpContext>();
    if(!context->parseRequest(buf, receiveTime)) {
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }

    if(context->gotAll()) {
        onRequest(conn, context->request());
        context->reset();
    }
}

void HttpServer::onRequest(const ideal::net::TcpConnectionPtr& conn, const HttpRequest& req) {
    const std::string& connection = req.getHeader("Connection");
    bool close = connection == "close" || 
                 (req.getVersion() == HttpRequest::kHttp10 && connection !=  "Keep-Alive");
    HttpResponse response(close);
    _httpCallback(req, &response);

    ideal::net::Buffer buf;
    response.appendToBuffer(&buf);
    conn->send(&buf);
    if(response.closeConnection()) {
        conn->shutdown();
    }
}

