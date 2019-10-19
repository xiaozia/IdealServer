
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : CountDownLatch.h
*   Last Modified : 2019-05-25 17:59
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_BASE_COUNTDOWNLATCH_H
#define  _IDEAL_BASE_COUNTDOWNLATCH_H

#include "NonCopyable.h"

#include <mutex>
#include <condition_variable>

namespace ideal {

class CountDownLatch : public NonCopyable {
public:
    explicit CountDownLatch(int count) :
        _mtx(),
        _cv(),
        _count(count) {
    }

    void wait() {
        std::unique_lock<std::mutex> ul(_mtx);
        _cv.wait(ul, [this] { return _count == 0; });
    }

    void countDown() {
        std::lock_guard<std::mutex> lg(_mtx);
        if(--_count == 0)
            _cv.notify_all();
    }

    int getCount() {
        std::lock_guard<std::mutex> lg(_mtx);
        return _count;
    }

private:
    std::mutex _mtx;
    std::condition_variable _cv;
    int _count;
};


}


#endif // _IDEAL_BASE_COUNTDOWNLATCH_H


