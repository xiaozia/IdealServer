
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : BoundedBlockingQueue.h
*   Last Modified : 2019-05-25 10:34
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_BASE_BOUNDEDBLOCKINGQUEUE_H
#define  _IDEAL_BASE_BOUNDEDBLOCKINGQUEUE_H

#include "noncopyable.h"
#include <boost/circular_buffer.hpp>

#include <deque>
#include <mutex>
#include <condition_variable>


namespace ideal {

template <class T>
class BoundedBlockingQueue : public noncopyable {
public:
    BoundedBlockingQueue(int maxSize) :
        _mtx(),
        _noEmpty(),
        _noFull(),
        _queue(maxSize) {
    }

    void put(const T& x) {
        std::unique_lock<std::mutex> ul(_mtx);
        _noFull.wait(ul, [this]{ return !_queue.full(); });

        _queue.push_back(x);
        _noEmpty.notify_all();
    }

    void put(T&& x) {
        std::unique_lock<std::mutex> ul(_mtx);
        _noFull.wait(ul, [this]{ return !_queue.full(); });

        _queue.push_back(std::move(x));
        _noEmpty.notify_all();
    }

    T take() {
        std::unique_lock<std::mutex> ul(_mtx);
        _noEmpty.wait(ul, [this]{ return !_queue.empty(); });

        T front(std::move(_queue.front()));
        _queue.pop_front();
        _noEmpty.notify_all();
        return std::move(front);
    }

    size_t size() const {
        std::lock_guard<std::mutex> lg(_mtx);
        return _queue.size();
    }

    size_t capacity() const {
        std::lock_guard<std::mutex> lg(_mtx);
        return _queue.capacity();
    }

private:
    std::mutex _mtx;
    std::condition_variable _noEmpty;
    std::condition_variable _noFull;
    boost::circular_buffer<T> _queue;

};

}


#endif // _IDEAL_BASE_BOUNDEDBLOCKINGQUEUE_H


