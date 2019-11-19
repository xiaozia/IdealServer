
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : HttpRequest.h
*   Last Modified : 2019-11-19 20:46
*   Describe      :
*
*******************************************************/

#ifndef  _EXAMPLE_HTTP_HTTPREQUEST_H
#define  _EXAMPLE_HTTP_HTTPREQUEST_H

#include "ideal/base/Copyable.h"
#include "ideal/base/Timestamp.h"

#include <string>
#include <map>
#include <assert.h>

class HttpRequest : public ideal::Copyable {
public:
    enum Method {
        kInvalid,
        kGet,
        kPost,
        kHead,
        kPut,
        kDelete
    };
    enum Version {
        kUnknown,
        kHttp10,
        kHttp11
    };
    HttpRequest() :
        _method(kInvalid),
        _version(kUnknown) { }

    void setVersion(Version v) { _version = v; }
    Version getVersion() const { return _version; }

    bool setMethod(const char* start, const char* end) {
        assert(_method == kInvalid);
        std::string s(start, end);
        if(s == "GET") {
            _method = kGet;
        }
        else if(s == "POST") {
            _method = kPost;
        }
        else if(s == "HEAD") {
            _method = kHead;
        }
        else if(s == "PUT") {
            _method = kPut;
        }
        else if(s == "DELETE") {
            _method = kDelete;
        }
        else {
            _method = kInvalid;
        }
        return _method != kInvalid;
    }
    Method getMethod() const { return _method; }
        
    const char* methodToString() const {
        const char* result = "UNKNOWN";
        switch(_method) {
            case kGet:
                result = "GET";
                break;
            case kPost:
                result = "POST";
                break;
            case kHead:
                result = "HEAD";
                break;
            case kPut:
                result = "PUT";
                break;
            case kDelete:
                result = "DELETE";
                break;
            default:
                break;
        }
        return result;
    }

    void setPath(const char* start, const char* end) { _path.assign(start, end); }
    const std::string& getPath() const { return _path; }

    void setQuery(const char* start, const char* end) { _query.assign(start, end); }
    const std::string& getQuery() const { return _query; }

    void setReceiveTime(ideal::Timestamp t) { _receiveTime = t; }
    ideal::Timestamp getReceiveTime() const { return _receiveTime; }

    void addHeader(const char* start, const char* colon, const char* end) {
        std::string field(start, colon);
        ++colon;
        while(colon < end && isspace(*colon)) {
            ++colon;
        }
        std::string value(colon, end);
        
        while(!value.empty() && isspace(value[value.size()-1])) {
            value.resize(value.size()-1);
        }
        _headers[field] = value;
    }
    std::string getHeader(const std::string& field) const {
        std::string result;
        std::map<std::string, std::string>::const_iterator iter = _headers.find(field);
        if(iter != _headers.end()) {
            result = iter->second;
        }
        return result;
    }
    const std::map<std::string, std::string> headers() const {
        return _headers;
    }

    void swap(HttpRequest& that) {
        std::swap(_method, that._method);
        std::swap(_version, that._version);
        _path.swap(that._path);
        _query.swap(that._query);
        _receiveTime.swap(that._receiveTime);
        _headers.swap(that._headers);
    }

private:
    Method _method;
    Version _version;
    std::string _path;
    std::string _query;
    ideal::Timestamp _receiveTime;
    std::map<std::string, std::string> _headers;
};

#endif // _EXAMPLE_HTTP_HTTPREQUEST_H


