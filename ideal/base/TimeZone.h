
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : TimeZone.h
*   Last Modified : 2019-06-04 13:56
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_BASE_TIMEZONE_H
#define  _IDEAL_BASE_TIMEZONE_H

#include "ideal/base/copyable.h"
#include <memory>
#include <time.h>

namespace ideal {

class TimeZone : public copyable {
public:
    explicit TimeZone(const char* zonefile);
    TimeZone(int eastOfUtc, const char* tzname);
    TimeZone() = default;

    bool valid() { return static_cast<bool>(_data); }

    struct tm toLocalTime(time_t secondsSinceEpoch) const;
    time_t fromLocalTime(const struct tm&) const;
    static struct tm toUtcTime(time_t secondsSinceEpoch, bool yday = false);
    static time_t fromUtcTime(const struct tm&);
    static time_t fromUtcTime(int year, int month, int day, int hour, int minute, int second);

    struct Data;

private:
    std::shared_ptr<Data> _data;
};

}

#endif // _IDEAL_BASE_TIMEZONE_H


