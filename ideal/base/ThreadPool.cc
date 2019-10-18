
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : ThreadPool.cc
*   Last Modified : 2019-06-06 11:29
*   Describe      :
*
*******************************************************/

#include "ideal/base/ThreadPool.h"
#include "ideal/base/Exception.h"

#include <cassert>

using namespace ideal;

ThreadPool::ThreadPool(const std::string& name) :
    _mtx(),
    _notEmpty(),
    _notFull(),
    _name(name),
    _maxQueueSize(0),
    _running(false) {

}

ThreadPool::~ThreadPool() {
    if(_running) {
        stop();
    }
}

void ThreadPool::start(int numThreads) {
    assert(_threads.empty());
    _running = true;
    for(int i=0; i<numThreads; ++i) {
        char id[32] = "";
        snprintf(id, sizeof id, "%d", i+1);
        _threads.emplace_back(new ideal::Thread(std::bind(&ThreadPool::runInThread, this), _name + id));
        _threads[i]->start();
    }
    if(numThreads == 0 && _threadInitCallback) {
        _threadInitCallback();
    }
}

void ThreadPool::stop() {
    {
        std::lock_guard<std::mutex> lg(_mtx);
        _running = false;
        _notEmpty.notify_all();
    }

    for(auto& t : _threads) {
        t->join();
    }
}

void ThreadPool::run(Task task) {
    if(_threads.empty()) {
        task();
    }
    else {
        std::unique_lock<std::mutex> ul(_mtx);
        if(_maxQueueSize > 0) {
            _notFull.wait(ul, [this] { return !(_queue.size() >= _maxQueueSize); });
        }
        _queue.push_back(std::move(task));
        _notEmpty.notify_all();
    } 
}

size_t ThreadPool::queueSize() const {
    std::lock_guard<std::mutex> lg(_mtx);
    return _queue.size();
}

void ThreadPool::runInThread() {
    try {
        if(_threadInitCallback) {
            _threadInitCallback();
        }
        while(_running) {
            Task task(take());
            if(task) {
                task();
            }
        }
    }
    catch(const Exception& ex) {
        fprintf(stderr, "exception caught in ThreadPool %s\n", _name.c_str()); // ?
        fprintf(stderr, "reason: %s\n", ex.what());
        fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
        abort();
    }
    catch(const std::exception& ex) {
        fprintf(stderr, "exception caught in ThreadPool %s\n", _name.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        abort();
    }
    catch(...) {
        fprintf(stderr, "unknown exception caught in ThreadPool %s\n", _name.c_str());
        throw;
    }
}

ThreadPool::Task ThreadPool::take() {
    std::unique_lock<std::mutex> ul(_mtx);
    _notEmpty.wait(ul, [this] { return !_queue.empty() || !_running; });
    Task task;
    if(!_queue.empty()) {
        task = _queue.front();
        _queue.pop_front();
        if(_maxQueueSize > 0) {
            _notFull.notify_all();
        }
    }
    return task;
}

