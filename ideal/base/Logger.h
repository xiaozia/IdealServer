
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Logger.h
*   Last Modified : 2019-11-22 20:50
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_BASE_LOGGER_H
#define  _IDEAL_BASE_LOGGER_H

#include "ideal/base/Timestamp.h"
#include "ideal/base/TimeZone.h"
#include "ideal/base/LogStream.h"

#include <string.h>
#include <assert.h>

namespace ideal {

// 用于格式化微秒字符串
class Fmt {
public:
    template <class T>
    Fmt(const char* fmt, T val) {
        static_assert(std::is_arithmetic<T>::value == true, "Must be arithmetic type");
        _len = snprintf(_buf, sizeof _buf, fmt, val);
        assert(static_cast<size_t>(_len) < sizeof _buf);
    }

    const char* data() const { return _buf; }
    int length() const { return _len; }

    friend LogStream& operator<<(LogStream& s, const Fmt& fmt) {
        s.append(fmt.data(), fmt.length());
        return s;
    }

private:
    char _buf[32];
    int _len;
};

class T {
public:
    T(const char* str, unsigned len) :
        _str(str),
        _len(len) {
        assert(strlen(str) == _len);
    }
    
    friend LogStream& operator<<(LogStream& s, T v) {
        s.append(v._str, v._len);
        return s;
    }

    const char* _str;
    const unsigned _len;
};


class Logger {
public:
    enum LogLevel {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        LOG_LEVEL_NUM
    };

// 处理__FILE__返回冗余文件名
class SourceFile {
public:
    // 数组型别
    template <int N>
    SourceFile(const char(&arr)[N]) :
        _data(arr),
        _size(N-1) {
   		const char* slash = strrchr(_data, '/');
      	if(slash) {
        	_data = slash + 1;
        	_size -= static_cast<int>(_data - arr);
      	}
	}
    // 指针型别
    explicit SourceFile(const char* filename) :
        _data(filename) {
        const char* slash = strrchr(filename, '/');
        if(slash) {
            _data = slash + 1;
        }
        _size = static_cast<int>(strlen(_data));
    }

    friend LogStream& operator<<(LogStream& s, const Logger::SourceFile& v) {
        s.append(v._data, v._size);
        return s;
    }

public:
    const char* _data;
    int _size;
};

    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, LogLevel level, const char* func);
    Logger(SourceFile file, int line, bool toAbort);
    ~Logger();

    LogStream& stream() { return _impl._stream; }

    typedef void (*OutputFunc)(const char* msg, int len);
    typedef void (*FlushFunc)();

    static void setLogLevel(LogLevel level);
    static void setOutput(OutputFunc);
    static void setFlush(FlushFunc);
    static void setTimeZone(const TimeZone& tz);
    
    static LogLevel logLevel();

private:
// Pimpl技法
class Impl {
public:
    using LogLevel = Logger::LogLevel;
    Impl(LogLevel level, int old_errno, const SourceFile& file, int line);
    
    void formatTime();
    void finish();

public:
    Timestamp _ts;
    LogStream _stream;
    LogLevel _level;
    int _line;
    SourceFile _basename;
};

    Impl _impl;
};


#define LOG_TRACE if(ideal::Logger::logLevel() <= ideal::Logger::TRACE) \
    ideal::Logger(__FILE__, __LINE__, ideal::Logger::TRACE, __func__).stream()
#define LOG_DEBUG if(ideal::Logger::logLevel() <= ideal::Logger::DEBUG) \
    ideal::Logger(__FILE__, __LINE__, ideal::Logger::DEBUG, __func__).stream()
#define LOG_INFO if(ideal::Logger::logLevel() <= ideal::Logger::INFO) \
    ideal::Logger(__FILE__, __LINE__).stream()

#define LOG_WARN ideal::Logger(__FILE__, __LINE__, ideal::Logger::WARN).stream()
#define LOG_ERROR ideal::Logger(__FILE__, __LINE__, ideal::Logger::ERROR).stream()
#define LOG_FATAL ideal::Logger(__FILE__, __LINE__, ideal::Logger::FATAL).stream()

#define LOG_SYSERR ideal::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL ideal::Logger(__FILE__, __LINE__, true).stream()

const char* strerror_tl(int savedErrno);

}

#endif // _IDEAL_BASE_LOGGER_H


