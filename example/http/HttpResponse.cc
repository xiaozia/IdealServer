
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : HttpResponse.cc
*   Last Modified : 2019-11-19 20:58
*   Describe      :
*
*******************************************************/

#include "HttpResponse.h"

#include "ideal/net/Buffer.h"

#include <stdio.h>

void HttpResponse::appendToBuffer(ideal::net::Buffer* output) const {
    char buf[32];
    snprintf(buf, sizeof buf, "HTTP/1.1 %d ", _statusCode);
    output->append(buf);
    output->append(_statusMessage);
    output->append("\r\n");

    if(_closeConnection) {
        output->append("Connection: close\r\n");
    }
    else {
        snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", _body.size());
        output->append(buf);
        output->append("Connection: kepp-Alive\r\n");
    }
    for(const auto& header : _headers) {
        output->append(header.first);
        output->append(": ");
        output->append(header.second);
        output->append("\r\n");
    }
    output->append("\r\n");
    output->append(_body);
}

