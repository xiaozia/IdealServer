
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : noncopyable.h
*   Last Modified : 2019-05-25 10:08
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_BASE_NONCOPYABLE_H
#define  _IDEAL_BASE_NONCOPYABLE_H

namespace ideal {

class noncopyable {
public:
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(noncopyable&) = delete;

protected:
    noncopyable() = default;
    ~noncopyable() = default;
};

}

#endif // _IDEAL_BASE_NONCOPYABLE_H


