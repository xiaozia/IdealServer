
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : client.cc
*   Last Modified : 2019-11-17 19:24
*   Describe      :
*
*******************************************************/

#include "ideal/base/Logger.h"
#include "ideal/base/NonCopyable.h"
#include "ideal/net/EventLoop.h"
#include "ideal/net/EventLoopThreadPool.h"
#include "ideal/net/TcpConnection.h"
#include "ideal/net/TcpClient.h"

#include <memory>
#include <cstdio>
#include <unistd.h>

using namespace ideal;
using namespace ideal::net;

class Client;

class Session : public NonCopyable {
public:
    Session(EventLoop* loop,
            const InetAddress& serverAddr,
            const std::string& name,
            Client* client) :
        _client(loop, serverAddr, name),
        _owner(client),
        _bytesRead(0),
        _messagesRead(0) {
        _client.setConnectionCallback(
                std::bind(&Session::onConnection, this, _1));
        _client.setMessageCallback(
                std::bind(&Session::onMessage, this, _1, _2, _3));
    }

    void start() {
        _client.connect();
    }

    void stop() {
        _client.disconnect();
    }

    int64_t bytesRead() const {
        return _bytesRead;
    }

    int64_t messagesRead() const {
        return _messagesRead;
    }

private:
    // 完整定义在Client之后，否则会报错，不然应该使用头文件
    void onConnection(const TcpConnectionPtr& conn);
   
    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp) {
        ++_messagesRead;
        _bytesRead += buf->readableBytes();
        conn->send(buf);
    }


private:
    TcpClient _client;
    Client* _owner;
    
    int64_t _bytesRead;
    int64_t _messagesRead;
};


class Client : public NonCopyable {
public:
    Client(EventLoop* loop,
           const InetAddress& serverAddr,
           int blockSize,
           int sessionCount,
           int timeout,
           int threadNum) :
        _loop(loop), 
        _threadPool(loop, "PingPongClient"),
        _sessionCount(sessionCount),
        _timeout(timeout) {
       if(threadNum > 1) {
            _threadPool.setThreadNum(threadNum);
        }
        _threadPool.start();

        for(int i = 0; i < blockSize; ++i) {
            _message.push_back(static_cast<char>(i % 128));
        }
        
        _loop->runAfter(timeout, std::bind(&Client::handleTimeout, this));
        
        for(int i = 0; i < sessionCount; ++i) {
            char buf[16] = { 0 };
            snprintf(buf, sizeof buf, "C%05d", i);
            Session* session = new Session(_threadPool.getNextLoop(), serverAddr, buf, this);
            session->start();
            _sessions.emplace_back(session);
        }
    }

    const std::string& message() const {
        return _message;
    }

    void onConnect() {
        if(++_numConnected == _sessionCount) {
            LOG_WARN << "all sessions connected";
        }
    }

    void onDisconnect(const TcpConnectionPtr& conn) {
        if(--_numConnected == 0) {
            LOG_WARN << "all sessions disconnected";
            
            int64_t totalBytesRead = 0;
            int64_t totalMessagesRead = 0;
            for(const auto& session : _sessions) {
                totalBytesRead += session->bytesRead();
                totalMessagesRead += session->messagesRead();
            }

            LOG_WARN << totalBytesRead << " total bytes read";
            LOG_WARN << totalMessagesRead << " total messages read";
            LOG_WARN << static_cast<double>(totalBytesRead) / static_cast<double>(totalMessagesRead) << " average message size";
            LOG_WARN << static_cast<double>(totalBytesRead) / (_timeout * 1024 * 1024)
                     << " MiB/s throughput";

            conn->getLoop()->queueInLoop(std::bind(&Client::quit, this));
        }
    }

private:
    void quit() {
        _loop->queueInLoop(std::bind(&EventLoop::quit, _loop));
    }

    void handleTimeout() {
        LOG_WARN << "stop";
        for(auto& session : _sessions) {
            session->stop();
        }
    }


private:
    EventLoop* _loop;
    EventLoopThreadPool _threadPool;
    
    int _sessionCount;
    int _timeout;

    std::vector<std::unique_ptr<Session>> _sessions;
    std::string _message;

    std::atomic_int _numConnected;
};

void Session::onConnection(const TcpConnectionPtr& conn) {
    if(conn->connected()) {
        conn->setTcpNoDelay(true);
        conn->send(_owner->message());
        _owner->onConnect();
    }
    else {
        _owner->onDisconnect(conn);
    }
}

int main(int argc, char* argv[]) {
    if(argc != 7) {
        fprintf(stderr, "Usage: client <server_ip> <server_port> <threads> "
                "<blocksize> <sessions> <time>\n");
    }
    else {
        Logger::setLogLevel(Logger::WARN);
        LOG_INFO << "pid = " << ::getpid() << ", tid = " << CurrentThread::tid();
    
        const char* ip = argv[1];
        uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
        int threadNum = atoi(argv[3]);
        int blockSize = atoi(argv[4]);
        int sessionCount = atoi(argv[5]);
        int timeout = atoi(argv[6]);

        EventLoop loop;
        InetAddress serverAddr(ip, port);

        Client client(&loop, serverAddr, blockSize, sessionCount, timeout, threadNum);
        loop.loop();
    }
}

