
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : CurrentThread.h
*   Last Modified : 2019-05-26 10:40
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_BASE_CURRENTTHREAD_H
#define  _IDEAL_BASE_CURRENTTHREAD_H

#include <string>

namespace ideal {

namespace CurrentThread {

extern __thread int t_cachedTid;
extern __thread char t_tidString[32];
extern __thread int t_tidStringLength;
extern __thread const char* t_threadName;

void getCachedTid();
bool isMainThread();
void sleepUsec(int64_t usec);
std::string stackTrace(bool demangle);

inline int tid() {
    if(t_cachedTid == 0) 
        getCachedTid();
    return t_cachedTid;
}

inline const char* tidString() {
    return t_tidString;
}

inline int tidStringLength() {
    return t_tidStringLength;
}

inline const char* name() {
    return t_threadName;
}

}

}

#endif // _IDEAL_BASE_CURRENTTHREAD_H


