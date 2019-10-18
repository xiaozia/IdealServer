
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Logger.cc
*   Last Modified : 2019-06-03 22:02
*   Describe      :
*
*******************************************************/

#include "ideal/base/Logger.h"
#include "ideal/base/CurrentThread.h"
#include "ideal/base/Timestamp.h"
#include "ideal/base/TimeZone.h"

#include <cerrno>

namespace ideal {

__thread time_t t_lastSecond;  // 缓存上一秒，避免频繁生成tm
__thread char t_time[64];      // 格式化日期时间
__thread char t_errnobuf[512]; // errno错误信息

const char* strerror_tl(int savedErrno) {
    return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
}

}



using namespace ideal;

template <class T>
Fmt::Fmt(const char* fmt, T val) {
    static_assert(std::is_arithmetic<T>::value == true, "Must be arithmetic type");
    _len = snprintf(_buf, sizeof _buf, fmt, val);
    assert(static_cast<size_t>(_len) < sizeof _buf);
}

LogStream& operator<<(LogStream& s, T v) {
    s.append(v._str, v._len);
    return s;
}

LogStream& operator<<(LogStream& s, const Logger::SourceFile& v) {
    s.append(v._data, v._size);
    return s;
}

const char* LogLevelName[Logger::LOG_LEVEL_NUM] = {
    "TRACE ",
    "DEBUG ",
    "INFO  ",
    "WARN  ",
    "ERROR ",
    "FATAL "
};

Logger::LogLevel initLogLevel() {
    if(::getenv("IDEAL_LOG_TRACE"))
        return Logger::TRACE;
    else if(::getenv("IDEAL_LOG_DEBUG"))
        return Logger::DEBUG;
    else
        return Logger::INFO;
}


void defaultOutput(const char* msg, int len) {
    size_t n = fwrite(msg, 1, len, stdout);
    (void)n;
}

void defaultFlush(){
    fflush(stdout);
}

TimeZone g_logTimeZone;
Logger::LogLevel g_logLevel = initLogLevel();
Logger::OutputFunc g_output = defaultOutput;
Logger::FlushFunc g_flush = defaultFlush;


Logger::Impl::Impl(LogLevel level, int savedErrno, const SourceFile& file, int line) :
    _ts(Timestamp::now()),
    _stream(),
    _level(level),
    _line(line),
    _basename(file) {
    formatTime();   // 时间ts
    CurrentThread::tid();   // 线程id
    _stream << T(CurrentThread::tidString(), CurrentThread::tidStringLength());
    _stream << T(LogLevelName[level], 6);   // 日志level
    if(savedErrno != 0) {  // 错误信息errno
        _stream << strerror_tl(savedErrno) << " (errno = " << savedErrno << ") ";
    }
}

void Logger::Impl::formatTime() {
    int64_t microsecondsSinceEpoch = _ts.microsecondsSinceEpoch();
    time_t seconds = static_cast<time_t>(microsecondsSinceEpoch / Timestamp::kMicrosecondsPerSecond);
    int microseconds = static_cast<int>(microsecondsSinceEpoch % Timestamp::kMicrosecondsPerSecond);
    if(seconds != t_lastSecond) {
        t_lastSecond = seconds;
        struct tm tm_time;
        if(g_logTimeZone.valid())
            tm_time = g_logTimeZone.toLocalTime(seconds);
        else
            ::gmtime_r(&seconds, &tm_time);
    
        int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d", 
                tm_time.tm_year+1900, tm_time.tm_mon+1, tm_time.tm_mday,
                tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
        assert(len == 17);
        (void)len;
    }

    if(g_logTimeZone.valid()) {
        Fmt us(".%06d ", microseconds);
        assert(us.length() == 8);
        _stream << T(t_time, 17) << T(us.data(), 8);
    }
    else {
        Fmt us(".%06dZ ", microseconds);
        assert(us.length() == 9);
        _stream << T(t_time, 17) << T(us.data(), 9);
    }
}

void Logger::Impl::finish() {
    _stream << " - " << _basename << ':' << _line << '\n'; // 文件名 行 换行
}

Logger::Logger(SourceFile file, int line) :
    _impl(INFO, 0, file, line) {
}

Logger::Logger(SourceFile file, int line, LogLevel level) :
    _impl(level, 0, file, line) {
}

Logger::Logger(SourceFile file, int line, LogLevel level, const char* func) :
    _impl(level, 0, file, line) {
    _impl._stream << func << ' ';
}

Logger::Logger(SourceFile file, int line, bool toAbort) :
    _impl(toAbort ? FATAL:ERROR, errno, file, line) {
}

Logger::~Logger() {
    _impl.finish();
    const LogStream::Buffer& buf(stream().buffer());
    g_output(buf.data(), buf.length());
    if(_impl._level == FATAL) {
        g_flush();
        abort();
    }
}

void Logger::setLogLevel(Logger::LogLevel level) {
    g_logLevel = level;
}

void Logger::setOutput(OutputFunc out) {
    g_output = out;
}

void Logger::setFlush(FlushFunc flush) {
    g_flush = flush;
}

void Logger::setTimeZone(const TimeZone& tz) {
    g_logTimeZone = tz;
}

Logger::LogLevel Logger::logLevel() {
    return g_logLevel;
}
