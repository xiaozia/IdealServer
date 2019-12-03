
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : loadtest.cc
*   Last Modified : 2019-12-01 13:22
*   Describe      :
*
*******************************************************/

#include "sudoku.h"
#include "percentile.h"

#include "ideal/base/NonCopyable.h"
#include "ideal/base/Types.h"
#include "ideal/base/Logger.h"
#include "ideal/base/FileUtil.h"
#include "ideal/net/Buffer.h"
#include "ideal/net/EventLoop.h"
#include "ideal/net/TcpClient.h"
#include "ideal/net/TcpConnection.h"

#include <fstream>
#include <memory>
#include <vector>
#include <unordered_map>

using namespace ideal;
using namespace ideal::net;

using Input = std::vector<std::string>;
using InputPtr = std::shared_ptr<const Input>;

InputPtr readInput(std::istream& in) {
    std::shared_ptr<Input> input(new Input);
    std::string line;
    while(getline(in, line)) {
        if(line.size() == implicit_cast<size_t>(kCells)) {
            input->push_back(line.c_str());
        }
    }
    return input;
}

class SudokuClient : public ideal::NonCopyable {
public:
    SudokuClient(EventLoop* loop,
                 const InetAddress& serverAddr,
                 const InputPtr& input,
                 const std::string& name,
                 bool nodelay) :
        _name(name),
        _tcpNoDelay(nodelay),
        _client(loop, serverAddr, name),
        _input(input),
        _count(0) {
        _client.setConnectionCallback(
            std::bind(&SudokuClient::onConnection, this, std::placeholders::_1));
        _client.setMessageCallback(
            std::bind(&SudokuClient::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }

    void connect() {
        _client.connect();
    }

    void send(int n) {
        assert(n > 0);
        if(!_conn)
            return;

        Timestamp now(Timestamp::now());
        for(int i = 0; i < n; ++i) {
            char buf[256] = { 0 };
            const std::string& req = (*_input)[_count % _input->size()];
            int len = snprintf(buf, sizeof buf, "%s-%08d:%s\r\n",
                               _name.c_str(), _count, req.c_str());

            _requests.append(buf, len);
            _sendTime[_count] = now;
            ++_count;
        }

        _conn->send(&_requests);
    }

    void report(std::vector<int>* latency, int* infly) {
        latency->insert(latency->end(), _latencies.begin(), _latencies.end());
        _latencies.clear();
        *infly += static_cast<int>(_sendTime.size());
    }

private:
    void onConnection(const TcpConnectionPtr& conn) {
    	if(conn->connected()) {
      		LOG_INFO << _name << " connected";
      		if(_tcpNoDelay)
        		conn->setTcpNoDelay(true);
      		_conn = conn;
    	}
    	else {
      		LOG_INFO << _name << " disconnected";
      		_conn.reset();
    	} 
    }

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp recvTime) {
		size_t len = buf->readableBytes();
		while(len >= kCells + 2) {
            const char* crlf = buf->findCRLF();
            if(crlf) {
                std::string response(buf->peek(), crlf);
                buf->retrieveUntil(crlf+2);
                len = buf->readableBytes();
                
                if(!verify(response, recvTime)) {
                    LOG_ERROR << "Bad response: " << response;
                    conn->shutdown();
                    break;
                }
            }
            else if(len > 100) {
                LOG_ERROR << "Line is too log!";
                conn->shutdown();
                break;
            }
            else {
                break;
            }
        } 
    }

    bool verify(const std::string& response, Timestamp recvTime) {
        size_t colon = response.find(':');
        if(colon != std::string::npos) {
            size_t dash = response.find('-');
            if(dash != std::string::npos && dash < colon) {
                int id = atoi(response.c_str() + dash + 1);
                std::unordered_map<int, Timestamp>::iterator sendTime = _sendTime.find(id);
                if(sendTime != _sendTime.end()) {
                    int64_t latency_us = recvTime.microsecondsSinceEpoch() - sendTime->second.microsecondsSinceEpoch();
                    _latencies.push_back(static_cast<int>(latency_us));
                   _sendTime.erase(sendTime);
                }
                else {
                    LOG_ERROR << "Unknown id " << id << " of " << _name;
                }
            }
        }
        return true;
    }


private:
    const std::string _name;
    const bool _tcpNoDelay;
    TcpClient _client;
    TcpConnectionPtr _conn;
    Buffer _requests;
    const InputPtr _input;
    int _count;
    std::unordered_map<int, Timestamp> _sendTime;
    std::vector<int> _latencies;
};


class SudokuLoadTest : public ideal::NonCopyable {
public:
    SudokuLoadTest() :
        _count(0),
        _ticks(0),
        _sofar(0) { }

    void runClient(const InputPtr& input, const InetAddress& serverAddr, int rps, int conn, bool nodelay) {
        EventLoop loop;

        for(int i = 0; i < conn; ++i) {
            Fmt f("c%04d", i+1);
            std::string name(f.data(), f.length());
            _clients.emplace_back(new SudokuClient(&loop, serverAddr, input, name, nodelay));
            _clients.back()->connect();
        }

        loop.runEvery(1.0/kHz, std::bind(&SudokuLoadTest::tick, this, rps));
        loop.runEvery(1.0, std::bind(&SudokuLoadTest::tock, this));
        loop.loop();
    }

private:
    void tick(int rps) { // rps均摊到0.01s
        ++_ticks;
        int64_t reqs = rps * _ticks / kHz - _sofar;
        _sofar += reqs;

        if(reqs > 0) {
            for(const auto& client : _clients) {
                client->send(static_cast<int>(reqs));
            }
        }
    }

    void tock() {
        std::vector<int> latencies;
        int infly = 0;
        for(const auto& client : _clients) {
            client->report(&latencies, &infly);
        }

        Percentile p(latencies, infly);
        LOG_INFO << p.report();

        char buf[64] = { 0 };
        snprintf(buf, sizeof buf, "r%04d", _count);
        p.save(latencies, buf);
        ++_count;
    }

private:
    static const int kHz = 100;

    std::vector<std::unique_ptr<SudokuClient>> _clients;
    int _count;
    int64_t _ticks;
    int64_t _sofar;
};


int main(int argc, char* argv[]) {
    int conn = 1;
    int rps = 100;
    bool nodelay = false;
    InetAddress serverAddr("127.0.0.1", 9981);

	switch(argc) {
		case 6:
	    	nodelay = std::string(argv[5]) == "-n";
	   	case 5:
	     	conn = atoi(argv[4]);
	   	case 4:
	     	rps = atoi(argv[3]);
	   	case 3:
	     	serverAddr = InetAddress(argv[2], 9981);
	   	case 2:
	   		break;
	   	default:
	     	printf("Usage: %s input server_ip [requests_per_second] [connections] [-n]\n", argv[0]);
    
        return 0;
    }

    std::ifstream in(argv[1]);
    if(in) {
        InputPtr input(readInput(in));
        printf("%zd requests from %s\n", input->size(), argv[1]);

        SudokuLoadTest test;
        test.runClient(input, serverAddr, rps, conn, nodelay);
    }
    else {
        printf("Can not open %s\n", argv[1]);
    }
}

