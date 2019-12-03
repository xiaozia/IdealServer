
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Thread.cc
*   Last Modified : 2019-11-23 14:13
*   Describe      :
*
*******************************************************/

#include "Thread.h"
#include "CurrentThread.h"
#include "Exception.h"

#include <memory>
#include <assert.h>

#include <sys/prctl.h>

namespace ideal {

class ThreadData {
public:
    using ThreadFunc = Thread::ThreadFunc;

    ThreadData(ThreadFunc func, const std::string& name, pid_t* tid, CountDownLatch* latch) :
        _func(std::move(func)),
        _name(name),
        _tid(tid),
        _latch(latch) {
    }

    void runInThread() {
        *_tid = ideal::CurrentThread::tid();
        _tid = nullptr;
        _latch->countDown();
        _latch = nullptr;

        ideal::CurrentThread::t_threadName = _name.empty()? "Thread" : _name.c_str();
        ::prctl(PR_SET_NAME, ideal::CurrentThread::t_threadName);

        try {
            _func();
            ideal::CurrentThread::t_threadName = "finished";
        }
        catch(const Exception& ex) {
            ideal::CurrentThread::t_threadName = "crashed";
            fprintf(stderr, "exception caught in Thread %s\n", _name.c_str());
            fprintf(stderr, "reason: %s\n", ex.what());
            fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
            abort();
        } 
        catch(const std::exception& ex) {
        	ideal::CurrentThread::t_threadName = "crashed";
      		fprintf(stderr, "exception caught in Thread %s\n", _name.c_str());
      		fprintf(stderr, "reason: %s\n", ex.what());
      		abort(); 
        }
        catch(...) {
      		ideal::CurrentThread::t_threadName = "crashed";
      		fprintf(stderr, "unknown exception caught in Thread %s\n", _name.c_str());
        	throw;
		}
    }

public:
    ThreadFunc _func;
    std::string _name;
    pid_t* _tid;
    CountDownLatch* _latch;
};

void startThread(std::shared_ptr<ThreadData> data)
{
    data->runInThread();
}

// 不可拷贝的
std::atomic<int> Thread::_numCreated(0);

Thread::Thread(ThreadFunc func, const std::string& name) :
    _started(false),
    _joined(false),
    _tid(0),
    _func(std::move(func)),
    _name(name),
    _latch(1) {
    setDefaultName();
}

Thread::~Thread() {
    if(_started && !_joined) {
        _thread.detach();
    }
}

void Thread::setDefaultName() {
    int num = ++_numCreated;
    if(_name.empty()) {
        char buf[32] = {0};
        snprintf(buf, sizeof(buf), "Thread%d", num);
        _name = buf;
    }
}

void Thread::start() {
    assert(!_started);
    _started = true;
    
    std::shared_ptr<ThreadData> data(std::make_shared<ThreadData>(_func, _name, &_tid, &_latch));
    std::thread t(startThread, data);
    _thread = std::move(t);

    _latch.wait();
}

void Thread::join() {
    assert(_started);
    assert(!_joined);

    _joined = true;
    _thread.join();
}

}


