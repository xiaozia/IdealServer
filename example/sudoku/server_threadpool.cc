
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : server_threadpool.cc
*   Last Modified : 2019-11-23 09:56
*   Describe      :
*
*******************************************************/

#include "sudoku.h"

#include "ideal/base/Types.h"
#include "ideal/base/Logger.h"
#include "ideal/base/ThreadPool.h"
#include "ideal/net/EventLoop.h"
#include "ideal/net/TcpServer.h"

#include <unistd.h>

using namespace ideal;
using namespace ideal::net;

class SudokuServer {
public:
    SudokuServer(EventLoop* loop, const InetAddress& listenAddr, int numThreads) :
        _server(loop, listenAddr, "SudokuServer"),
        _numThreads(numThreads) {
        _server.setConnectionCallback(
            std::bind(&SudokuServer::onConnection, this, _1));
        _server.setMessageCallback(
            std::bind(&SudokuServer::onMessage, this, _1, _2, _3));
    }

    void start() {
        LOG_INFO << "starting " << _numThreads << " threads";
        _threadPool.start(_numThreads);
        _server.start();
    }

private:
    void onConnection(const TcpConnectionPtr& conn) {
        LOG_TRACE << conn->peerAddress().toIpPort() << " -> "
                  << conn->localAddress().toIpPort() << " is "
                  << (conn->connected()? "UP" : "DOWN");
    }

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp) {
        LOG_DEBUG << conn->name();
        size_t len = buf->readableBytes();
        while(len >= kCells + 2) {
            const char* crlf = buf->findCRLF();
            if(crlf) {
                std::string request(buf->peek(), crlf);
                buf->retrieveUntil(crlf + 2);
                len = buf->readableBytes();

                if(!processRequest(conn, request)) {
                    conn->send("Bad Request!\r\n");
                    conn->shutdown();
                    break;
                }
            }
            else if(len > 100) {
                conn->send("Id too long!\r\n");
                conn->shutdown();
                break;
            }
            else {
                break;
            }
        }
    }

    bool processRequest(const TcpConnectionPtr& conn, const std::string& request) {
        std::string id;
        std::string puzzle;
        bool goodRequest = true;

        std::string::const_iterator colon = find(request.begin(), request.end(), ':');
        if(colon != request.end()) {
            id.assign(request.begin(), colon);
            puzzle.assign(colon+1, request.end());
        }
        else {
            puzzle = request;
        }

        if(puzzle.size() == implicit_cast<size_t>(kCells)) {
//            LOG_DEBUG << conn->name();
//            std::string result = solveSudoku(puzzle);
//            if(id.empty()) {
//                conn->send(result + "\r\n");
//            }
//            else {
//                conn->send(id + ":" + result + "\r\n");
//            }
            _threadPool.run(std::bind(&solve, conn, puzzle, id));
        }
        else {
            goodRequest = false;
        }
        return goodRequest;
    }

    static void solve(const TcpConnectionPtr& conn, const std::string& puzzle, const std::string& id) {
        LOG_DEBUG << conn->name();
        std::string result = solveSudoku(puzzle);
        if(id.empty()) {
            conn->send(result + "\r\n");
        }
       else {
            conn->send(id + ":" + result + "\r\n");
        }
    }

private:
    TcpServer _server;
    ThreadPool _threadPool;
    int _numThreads;
};


int main(int argc, char* argv[]) {
    LOG_INFO << "pid = " << ::getpid() << ", tid = " << CurrentThread::tid();
    int numThreads = 0;
    if(argc > 1)
        numThreads = atoi(argv[1]);

    EventLoop loop;
    InetAddress listenAddr(9981);
    SudokuServer server(&loop, listenAddr, numThreads);
    server.start();
    loop.loop();
}


