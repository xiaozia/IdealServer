
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : LogFile.h
*   Last Modified : 2019-06-06 19:52
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_BASE_LOGFILE_H
#define  _IDEAL_BASE_LOGFILE_H

#include "ideal/base/FileUtil.h"

#include <string>
#include <memory>
#include <mutex>

namespace ideal {

class LogFile {
public:
    LogFile(const std::string& basename,
            off_t rollsize,
            bool threadSafe = true,
            int flushInterval = 3,
            int checkEvertN = 1024);

    ~LogFile();

    void append(const char* logline, int len);
    void flush();
    bool rollFile();

private:
    void append_unlocked(const char* logline, int len);

    static std::string getLogFileName(const std::string& basename, time_t* now);

private:
    const std::string _basename;
    const off_t _rollSize;
    const int _flushInterval;
    const int checkEveryN;
    int _count;

    std::unique_ptr<std::mutex> _spmtx;
    time_t _startOfPeriod;
    time_t _lastRoll;
    time_t _lastFlush;
    std::unique_ptr<FileUtil::AppendFile> _file;

    const static int kRollPerSeconds = 60 * 60 * 24;
};

}

#endif // _IDEAL_BASE_LOGFILE_H


