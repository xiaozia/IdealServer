
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : HttpResponse.h
*   Last Modified : 2019-11-19 20:57
*   Describe      :
*
*******************************************************/

#ifndef  _EXAMPLE_HTTP_HTTPRESPONSE_H
#define  _EXAMPLE_HTTP_HTTPRESPONSE_H

#include "ideal/base/Copyable.h"

#include <map>

namespace ideal {
namespace net {
class Buffer;
}
}

class HttpResponse : public ideal::Copyable {
public:
    enum HttpStatusCode {
        kUnknown,
        k200Ok = 200,
        k301MovedPermanently = 301,
        k400BadRequest = 400,
        k404NotFound = 404
    };

    explicit HttpResponse(bool close) :
        _statusCode(kUnknown),
        _closeConnection(close) {
    }

    void setStatusCode(HttpStatusCode code) { _statusCode = code; }
    void setStatusMessage(const std::string& message) { _statusMessage = message; }
    
    void setCloseConnection(bool on) { _closeConnection = on; }
    bool closeConnection() const { return _closeConnection; }

    void setContentType(const std::string& contentType) { addHeader("Content-Type", contentType); }
    void addHeader(const std::string& key, const std::string& value) { _headers[key] = value; }
    void setBody(const std::string& body) { _body = body; }
    
    void appendToBuffer(ideal::net::Buffer* output) const;

private:
    std::map<std::string, std::string> _headers;
    HttpStatusCode _statusCode;
    std::string _statusMessage;
    bool _closeConnection;
    std::string _body;
};

#endif // _EXAMPLE_HTTP_HTTPRESPONSE_H


