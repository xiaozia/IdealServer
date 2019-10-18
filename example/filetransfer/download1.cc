
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : download.cc
*   Last Modified : 2019-07-12 16:09
*   Describe      :
*
*******************************************************/

#include "ideal/base/Logger.h"
#include "ideal/net/EventLoop.h"
#include "ideal/net/TcpServer.h"

#include <stdio.h>
#include <unistd.h>

using namespace ideal;
using namespace ideal::net;

const char* g_file = NULL;

std::string readFile(const char* filename) {
    std::string content;
    FILE* fp = ::fopen(filename, "rb");
    if(fp) {
        const int kBufSize = 1024*1014;
        char iobuf[kBufSize];
        ::setbuffer(fp, iobuf, sizeof iobuf);

        char buf[kBufSize];
        size_t nread = 0;
        while((nread = ::fread(buf, 1, sizeof buf, fp)) > 0) {
            content.append(buf, nread);
        }
        ::fclose(fp);
    }
    return content;
}



void onHighWaterMark(const TcpConnectionPtr& conn, size_t len) {
    LOG_INFO << "HighWaterMark " << len;
}

void onConnection(const TcpConnectionPtr& conn) {
    LOG_INFO << "FileServer - " << conn->peerAddress().toIpPort() << " -> "
             << conn->localAddress().toIpPort() << " is "
             << (conn->connected()? "UP" : "DOWN");
    if(conn->connected()) {
        LOG_INFO << "FileServer - Send file " << g_file
                 << " to " << conn->peerAddress().toIpPort();
        conn->setHighWaterMarkCallback(onHighWaterMark, 64*1024);
        std::string fileContent = readFile(g_file);
        conn->send(fileContent);
        conn->shutdown();
        LOG_INFO << "FileServer - Send done";
    }
}

int main(int argc, char* argv[]) {
    LOG_INFO << "pid = " << getpid();
    if(argc > 1) {
        g_file = argv[1];

        EventLoop loop;
        InetAddress listenAddr(2021);
        TcpServer server(&loop, listenAddr, "FileServer");
        server.setConnectionCallback(onConnection);
        server.start();
        loop.loop();
    }
    else {
        fprintf(stderr, "Usage: %s file_for_downloading\n", argv[0]);
    }
}


