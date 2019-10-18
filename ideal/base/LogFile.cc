
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : LogFile.cc
*   Last Modified : 2019-06-09 20:18
*   Describe      :
*
*******************************************************/

#include "ideal/base/LogFile.h"
#include "ideal/base/ProcessInfo.h"

#include <assert.h>

using namespace ideal;

LogFile::LogFile(const std::string& basename,
                 off_t rollsize,
                 bool threadSafe,
                 int flushInterval,
                 int checkEveryN) :
    _basename(basename),
    _rollSize(rollsize),
    _flushInterval(flushInterval),
    checkEveryN(checkEveryN),
    _count(0),
    _spmtx(threadSafe? new std::mutex : nullptr),
    _startOfPeriod(0),
    _lastRoll(0),
    _lastFlush(0) {
    assert(basename.find('/') == std::string::npos);
    rollFile();
}

LogFile::~LogFile() = default;

void LogFile::append(const char* logline, int len) {
    if(_spmtx) {
        std::lock_guard<std::mutex> lg(*_spmtx);
        append_unlocked(logline, len);
    }
    else {
        append_unlocked(logline, len);
    }
}

void LogFile::flush() {
    if(_spmtx) {
        std::lock_guard<std::mutex> lg(*_spmtx);
        _file->flush();
    }
    else {
        _file->flush();
    }
}

bool LogFile::rollFile() {
    time_t now = 0;
    std::string filename = getLogFileName(_basename, &now);
    time_t start = now / kRollPerSeconds * kRollPerSeconds;

    if(now > _lastRoll) {
        _lastRoll = now;
        _lastFlush = now;
        _startOfPeriod = start;
        _file.reset(new FileUtil::AppendFile(filename));
        return true;
    }
    return false;
}

void LogFile::append_unlocked(const char* logline, int len) {
    _file->append(logline, len);
    if(_file->writtenBytes() > _rollSize) {
        rollFile();
    }
    else {
        ++_count;
        if(_count >= checkEveryN) {
            _count = 0;
            time_t now = ::time(NULL);
            time_t thisPeriod = now / kRollPerSeconds * kRollPerSeconds;
            if(thisPeriod != _startOfPeriod) {
                rollFile();
            }
            else if(now - _lastFlush > _flushInterval) {
                _lastFlush = now;
                _file->flush();
            }
        }
    }
}

std::string LogFile::getLogFileName(const std::string& basename, time_t* now) {
    std::string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;

    char timebuf[32];
    struct tm tm;
    *now = time(NULL);
    gmtime_r(now, &tm);
    strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);
    filename += timebuf;

    filename += ProcessInfo::hostname();

    char pidbuf[32];
    snprintf(pidbuf, sizeof pidbuf, ".%d", ProcessInfo::pid());
    filename += pidbuf;

    filename += ".log";
    
    return filename;
}

