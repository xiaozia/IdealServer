
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : download2.cc
*   Last Modified : 2019-11-06 20:36
*   Describe      :
*
*******************************************************/

#include "ideal/base/Logger.h"
#include "ideal/net/EventLoop.h"
#include "ideal/net/TcpServer.h"

#include <stdio.h>
#include <unistd.h>
#include <limits.h>

using namespace ideal;
using namespace ideal::net;

const char* g_file = NULL;
const int kBufSize = 64*1024;

void onHighWaterMark(const TcpConnectionPtr& conn, size_t len) {
	LOG_INFO << "HighWaterMark " << len;
}

void onConnection(const TcpConnectionPtr& conn) {
	LOG_INFO << "FileServer - " << conn->peerAddress().toIpPort() << " -> "
             << conn->localAddress().toIpPort() << " is "
             << (conn->connected() ? "UP" : "DOWN");
  	if(conn->connected()) {
    	LOG_INFO << "FileServer - Sending file " << g_file
                 << " to " << conn->peerAddress().toIpPort();
    	conn->setHighWaterMarkCallback(onHighWaterMark, kBufSize+1);

    	FILE* fp = ::fopen(g_file, "rb");
    	if(fp) {
      		conn->setContext(fp);

            // 文件名(255) + 文件大小(4) + 文件内容(文件大小)
            fseek(fp, 0, SEEK_END);
            int filesize = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            printf("%d\n", filesize);
            
            char tmp[NAME_MAX + sizeof(int)];
            strcpy(tmp, strrchr(g_file, '/')? strrchr(g_file, '/')+1 : g_file);
            memcpy(tmp + NAME_MAX, &filesize, sizeof(int));    // memcpy
            
            conn->send(tmp, NAME_MAX + sizeof(int));
    	}
    	else {
      		conn->shutdown();
      		LOG_INFO << "FileServer - no such file";
    	}
  	}
  	else {
    	if(!conn->getContext().empty()) {
      		FILE* fp = conn->getContext().any_cast<FILE*>();
      		if(fp) {
        		::fclose(fp);
      		}
		}
  	}
}

void onWriteComplete(const TcpConnectionPtr& conn)
{
	FILE* fp = conn->getContext().any_cast<FILE*>();
	char buf[kBufSize];
  	
	size_t nread = ::fread(buf, 1, sizeof buf, fp);
  	if(nread > 0) {
    conn->send(buf, static_cast<int>(nread));
  	}
  	else {
    	::fclose(fp);
    	fp = NULL;
    	conn->setContext(fp);
    	conn->shutdown();
    	LOG_INFO << "FileServer - done";
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
    	server.setWriteCompleteCallback(onWriteComplete);
    	server.start();
    	loop.loop();
  	}
  	else {
    	fprintf(stderr, "Usage: %s file_for_downloading\n", argv[0]);
  	}
}




