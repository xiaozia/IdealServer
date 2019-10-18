
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : LogStream.cc
*   Last Modified : 2019-05-26 18:58
*   Describe      :
*
*******************************************************/

#include "ideal/base/LogStream.h"

#include <algorithm>

namespace ideal {

const char digits[] = "9876543210123456789";
const char* zero = digits + 9; // 取余可能得到负数
const char digitsHex[] = "0123456789ABCDEF";

static_assert(sizeof(digits) == 20, "wrong number of digits");
static_assert(sizeof(digitsHex) == 17, "wrong number of digitsHex");

template <class T>
size_t convert(char buf[], T value) {
    T i = value;
    char* p = buf;

    do {
        int lsd = static_cast<int>(i%10);
        i /= 10;
        *p++ = zero[lsd];
    } while(i != 0);

    if(value < 0) {
        *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

size_t convertHex(char buf[], uintptr_t value) {
    uintptr_t i = value;
    char* p = buf;

    do {
        int lsd = static_cast<int>(i%16);
        i /= 16;
        *p++ = digitsHex[lsd];
    } while(i != 0);

    *p = '\0';
    std::reverse(buf, p);
    return p - buf;
}

template <class T>
void LogStream::formatInteger(T v) {
    if(_buffer.avail() >= kMaxNumericSize) {
        size_t len = convert(_buffer.current(), v);
        _buffer.add(len);
    }
}

LogStream& LogStream::operator<<(bool v) {
    _buffer.append(v? "1" : "0", 1);
    return *this;
}

LogStream& LogStream::operator<<(short v) {
    *this << static_cast<int>(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned short v) {
    *this << static_cast<unsigned int>(v);
    return *this;
}

LogStream& LogStream::operator<<(int v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned int v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long long v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long long v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(float v) {
    *this << static_cast<double>(v);
    return *this;
}

LogStream& LogStream::operator<<(double v) {
    if(_buffer.avail() >= kMaxNumericSize) {
        int len = snprintf(_buffer.current(), kMaxNumericSize, "%.12g", v);
        _buffer.add(len);
    }
    return *this;
}

LogStream& LogStream::operator<<(char v) {
    _buffer.append(&v, 1);
    return *this;
}

LogStream& LogStream::operator<<(const char* str) {
    if(str)
        _buffer.append(str, strlen(str));
    else
        _buffer.append("(null)", 6);

    return *this;
}

LogStream& LogStream::operator<<(const unsigned char* str) {
    *this << reinterpret_cast<const char*>(str);
    return *this;
}

LogStream& LogStream::operator<<(const void* p) {
    uintptr_t v = reinterpret_cast<uintptr_t>(p);
    if(_buffer.avail() >= kMaxNumericSize) {
        char* buf = _buffer.current();
        buf[0] = '0';
        buf[1] = 'x';
        size_t len = convertHex(buf+2, v);
        _buffer.add(len+2);
    }
    return *this;
}

LogStream& LogStream::operator<<(const std::string& str) {
    _buffer.append(str.c_str(), str.size());
    return *this;
}

LogStream& LogStream::operator<<(const StringPiece& str) {
    _buffer.append(str.data(), str.size());
    return *this;
}

LogStream& LogStream::operator<<(const Buffer& buf) {
    *this << buf.toStringPiece();
    return *this;
}

}

