
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : NonCopyable.h
*   Last Modified : 2019-05-25 10:08
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_BASE_NONCOPYABLE_H
#define  _IDEAL_BASE_NONCOPYABLE_H

namespace ideal {

class NonCopyable {
public:
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(NonCopyable&) = delete;

protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
};

}

#endif // _IDEAL_BASE_NONCOPYABLE_H


