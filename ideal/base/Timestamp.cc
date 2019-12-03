
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Timestamp.cc
*   Last Modified : 2019-11-23 14:13
*   Describe      :
*
*******************************************************/

#include "Timestamp.h"

#include <chrono>
#include <stdio.h>

// PRId64
#include <inttypes.h>
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

using namespace ideal;

std::string Timestamp::toString() const {
    char buf[32] = {0};
    int64_t seconds = _microsecondsSinceEpoch / kMicrosecondsPerSecond;
    int64_t microseconds = _microsecondsSinceEpoch % kMicrosecondsPerSecond;
    snprintf(buf, sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
    return buf;
}

std::string Timestamp::toFormattedString(bool showMicroseconds) const {
    char buf[64] = {0};
 	//此处转化为东八区北京时间，如果是其它时区需要按需求修改
	int64_t microseconds = _microsecondsSinceEpoch + 8*60*60*1000;
    time_t seconds = static_cast<time_t>(microseconds / kMicrosecondsPerSecond);
    struct tm tm_time;
    gmtime_r(&seconds, &tm_time);

    if(showMicroseconds) {
        int microseconds = static_cast<int>(_microsecondsSinceEpoch % kMicrosecondsPerSecond);
        snprintf(buf, sizeof(buf)-1, "%4d%02d%02d %02d:%02d:%02d.%06d", tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec, microseconds);
    }
    else {
        snprintf(buf, sizeof(buf)-1, "%4d%02d%02d %02d:%02d:%02d", tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    }
	return buf;
}

Timestamp Timestamp::now() {
    int64_t microsecondsSinceEpoch =  std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return Timestamp(microsecondsSinceEpoch);
}


