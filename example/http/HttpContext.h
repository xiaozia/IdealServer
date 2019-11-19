
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : HttpContext.h
*   Last Modified : 2019-11-19 20:00
*   Describe      :
*
*******************************************************/

#ifndef  _EXAMPLE_HTTP_HTTPCONTEXT_H
#define  _EXAMPLE_HTTP_HTTPCONTEXT_H

#include "HttpRequest.h"

#include "ideal/base/Copyable.h"

namespace ideal {
namespace net {
    class Buffer;
}
}

class HttpContext : public ideal::Copyable {
public:
    enum HttpRequestParseState {
        kExpectRequestLine,
        kExpectHeaders,
        kExpectBody,
        kGotAll
    };

    HttpContext() :
        _state(kExpectRequestLine) {
    }

    bool parseRequest(ideal::net::Buffer* buf, ideal::Timestamp receiveTime);

    bool gotAll() const { return _state == kGotAll; }

    void reset() {
        _state = kExpectRequestLine;
        HttpRequest dummy;
        _request.swap(dummy);
    }

    const HttpRequest& request() const { return _request; }
    HttpRequest& request() { return _request; }

private:
    bool processRequestLine(const char* begin, const char* end);

private:
    HttpRequestParseState _state;
    HttpRequest _request;
};

#endif // _EXAMPLE_HTTP_HTTPCONTEXT_H


