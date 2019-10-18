
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : ProcessInfo.h
*   Last Modified : 2019-06-09 20:30
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_BASE_PROCESSINFO_H
#define  _IDEAL_BASE_PROCESSINFO_H

#include "ideal/base/Timestamp.h"
#include "ideal/base/StringPiece.h"

#include <string>
#include <vector>

namespace ideal {

namespace ProcessInfo{

pid_t pid();
std::string pidString();
uid_t uid();
std::string username();
uid_t euid();
Timestamp startTime();
int clockTicksPerSecond();
int pageSize();
bool idDebugBuild();

std::string hostname();
std::string procname();
StringPiece procname(const std::string& stat);

std::string procStatus();
std::string procStat();
std::string threadStat();
std::string exePath();

int openedFiles();
int maxOpenFiles();

struct CpuTime {
    double userSeconds;
    double systemSeconds;

    CpuTime() : userSeconds(0.0), systemSeconds(0.0) { }
    double total() const {  return userSeconds + systemSeconds; }
};

CpuTime cpuTime();

int numThreads();
std::vector<pid_t> threads();

}

}

#endif // _IDEAL_BASE_PROCESSINFO_H


