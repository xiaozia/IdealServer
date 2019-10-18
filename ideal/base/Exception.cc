
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Exception.cc
*   Last Modified : 2019-05-26 15:05
*   Describe      :
*
*******************************************************/

#include "Exception.h"
#include "CurrentThread.h"

#include "string"

namespace ideal {

Exception::Exception(std::string msg) :
    _message(std::move(msg)),
    _stack(ideal::CurrentThread::stackTrace(false)) {
}


}


