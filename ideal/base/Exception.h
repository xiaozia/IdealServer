
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Exception.h
*   Last Modified : 2019-05-26 13:05
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_BASE_EXCEPTION_H
#define  _IDEAL_BASE_EXCEPTION_H

#include <exception>
#include <string>


namespace ideal {

class Exception : public std::exception {
public:
    Exception(std::string what);
    ~Exception() noexcept override = default;

    const char* what() const noexcept override {
        return _message.c_str();
    }

    const char* stackTrace() const noexcept {
        return _stack.c_str();
    }

private:
    std::string _message;
    std::string _stack;
};


}

#endif // _IDEAL_BASE_EXCEPTION_H


