
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : LogStream.h
*   Last Modified : 2019-05-25 21:18
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_BASE_LOGSTREAM_H
#define  _IDEAL_BASE_LOGSTREAM_H

#include "ideal/base/noncopyable.h"
#include "ideal/base/StringPiece.h"

#include <string>
#include <cstring>

namespace ideal {

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000*100;

template <int SIZE>
class FixedBuffer : public noncopyable {
public:
    FixedBuffer() :
        _cur(_data) {
    }

    ~FixedBuffer() {
    }

    void append(const char* buf, size_t len) {
        if(static_cast<size_t>(avail()) > len) {
            memcpy(_cur, buf, len);
            _cur += len;
        }
    }

    const char* data() const { return _data; }
    int length() const { return static_cast<int>(_cur - _data); }
    int avail() const { return static_cast<int>(end() - _cur); }
    char* current() const { return _cur; }
    void add(size_t len) { _cur += len; }
    void reset() { _cur = _data; }
    void bzero() { memset(_data, 0, sizeof(_data)); }

    std::string toString() const { return std::string(_data, length()); }
    StringPiece toStringPiece() const { return StringPiece(_data, length()); }

 private:
    const char* end() const { return _data + sizeof(_data); }

private:
    char _data[SIZE];
    char* _cur;
};


class LogStream : public noncopyable {
public:
    using Buffer = FixedBuffer<kSmallBuffer>;

    LogStream& operator<<(bool);
    LogStream& operator<<(short);
    LogStream& operator<<(unsigned short);
    LogStream& operator<<(int);
    LogStream& operator<<(unsigned int);
    LogStream& operator<<(long);
    LogStream& operator<<(unsigned long);
    LogStream& operator<<(long long);
    LogStream& operator<<(unsigned long long);
    LogStream& operator<<(float);
    LogStream& operator<<(double);
    LogStream& operator<<(char);
    LogStream& operator<<(const char*);
    LogStream& operator<<(const unsigned char*);
    LogStream& operator<<(const void*);
    LogStream& operator<<(const std::string&);
    LogStream& operator<<(const StringPiece&);
    LogStream& operator<<(const Buffer&);

    const Buffer& buffer() const { return _buffer; }
    void append(const char* data, int len) { _buffer.append(data, len); }
    void resetBuffer() { _buffer.reset(); }
        
private:
    template <class T>
    void formatInteger(T);

private:
    static const int kMaxNumericSize = 32;
    Buffer _buffer;
};


}


#endif // _IDEAL_BASE_LOGSTREAM_H


