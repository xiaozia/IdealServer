
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : BlockingQueue.h
*   Last Modified : 2019-05-25 10:18
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_BASE_BLOCKINGQUEUE_H
#define  _IDEAL_BASE_BLOCKINGQUEUE_H

#include "NonCopyable.h"

#include <queue>
#include <mutex>
#include <condition_variable>

namespace ideal {

template <class T>
class BlockingQueue : public NonCopyable {
public:
    BlockingQueue() :
        _mtx(),
        _cv(),
        _queue() {
    }

    void put(const T& x) {
        std::lock_guard<std::mutex> lg(_mtx);
        _queue.push_back(x);
        _cv.notify_all();
    }

    void put(T&& x) {
        std::lock_guard<std::mutex> lg(_mtx);
        _queue.push_back(std::move(x));
        _cv.notify_all();
    }

    T take() {
        std::unique_lock<std::mutex> ul(_mtx);
        _cv.wait(ul, [this]{ return !_queue.empty(); });

        T front(std::move(_queue.front()));
        _queue.pop_front();
        return std::move(front);
    }

    size_t size() {
        std::lock_guard<std::mutex> lg(_mtx);
        return _queue.size();
    }

private:
    std::mutex _mtx;
    std::condition_variable _cv;
    std::deque<T> _queue;
};

}

#endif // _IDEAL_BASE_BLOCKINGQUEUE_H


