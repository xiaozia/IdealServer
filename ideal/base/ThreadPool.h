
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : ThreadPool.h
*   Last Modified : 2019-06-06 11:13
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_BASE_THREADPOOL_H
#define  _IDEAL_BASE_THREADPOOL_H

#include "ideal/base/NonCopyable.h"
#include "ideal/base/Thread.h"

#include <string>
#include <deque>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace ideal {

class ThreadPool : public NonCopyable {
public:
    using Task = std::function<void()>;

    explicit ThreadPool(const std::string& name = "ThreadPool");
    ~ThreadPool();

    void setMaxQueueSize(int maxSize) { _maxQueueSize = maxSize; }
    void setThreadInitCallback(const Task& cb) { _threadInitCallback = cb; }

    void start(int numThreads);
    void stop();
    void run(Task f);

    const std::string& name() const { return _name; }
    size_t queueSize() const;

private:
    bool isFull() const;
    void runInThread();
    Task take();

private:
    mutable std::mutex _mtx;
    std::condition_variable _notEmpty;
    std::condition_variable _notFull;
    std::string _name;
    Task _threadInitCallback;
    std::vector<std::unique_ptr<ideal::Thread>> _threads;
    std::deque<Task> _queue;
    size_t _maxQueueSize;
    bool _running;
};

}

#endif // _IDEAL_BASE_THREADPOOL_H


