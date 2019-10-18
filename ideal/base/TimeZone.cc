
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : TimeZone.cc
*   Last Modified : 2019-06-04 14:05
*   Describe      :
*
*******************************************************/

#include "ideal/base/noncopyable.h"
#include "ideal/base/TimeZone.h"
#include "ideal/base/Date.h"

#include <stdexcept>
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
#include <string.h>
#include <time.h>

using namespace ideal;

const int kSecondsPerDay = 24 * 60 * 60;

struct Transition {
    time_t gmttime;
    time_t localtime;
    int localtimeIdx;

    Transition(time_t g, time_t l, int index) :
        gmttime(g),
        localtime(l),
        localtimeIdx(index) {
    }
};

struct Localtime {
    time_t gmtOffset;
    bool isDst;
    int arrbIdx;

    Localtime(time_t offset, bool dst, int index) :
        gmtOffset(offset),
        isDst(dst),
        arrbIdx(index) {
    }
};

struct Comp {
    bool compareGmt;
    Comp(bool gmt) :
        compareGmt(gmt) {
    }

    bool operator()(const Transition& lhs, const Transition& rhs) const {
        if(compareGmt)
            return lhs.gmttime < rhs.gmttime;
        else
            return lhs.localtime < rhs.localtime;
    }

    bool equal(const Transition& lhs, const Transition& rhs) const {
        if(compareGmt)
            return lhs.gmttime == rhs.gmttime;
        else
            return lhs.localtime == rhs.localtime;
    }
};

struct TimeZone::Data {
    std::vector<Transition> transitions;
    std::vector<Localtime> localtimes;
    std::vector<std::string> names;
    std::string abbreviation;
};


class File : public noncopyable {
public:
    File(const char* file) :
        _fp(::fopen(file, "rb")) {
    }

    ~File() {
        if(_fp) { ::fclose(_fp); }
    }

    bool valid() const { return _fp; }

    std::string readBytes(int n) {
        char buf[n];
        ssize_t nr = ::fread(buf, 1, n, _fp);
        if(nr != n)
            throw std::logic_error("no enough data");
        return std::string(buf, n);
    }

    int32_t readInt32() {
        int32_t x = 0;
        ssize_t nr = ::fread(&x, 1, sizeof(int32_t), _fp);
        if(nr != sizeof(int32_t))
            throw std::logic_error("bad int32_t data");
        return be32toh(x);
    }

    uint8_t readUint8() {
        uint8_t x = 0;
        ssize_t nr = ::fread(&x, 1, sizeof(uint8_t), _fp);
        if(nr != sizeof(uint8_t))
            throw std::logic_error("bad uint8_t data");
        return x;
    }

private:
    FILE* _fp;
};



void fillHMS(unsigned seconds, struct tm* utc) {
    utc->tm_sec = seconds % 60;
    unsigned minutes = seconds / 60;
    utc->tm_min = minutes % 60;
    utc->tm_hour = minutes / 60;
}


bool readTimeZoneFile(const char* zonefile, struct TimeZone::Data* data) {
    File f(zonefile);
    if(f.valid()) {
        try {
            std::string header = f.readBytes(4);
            if(header != "TZif")
                throw std::logic_error("bad header");
            std::string version = f.readBytes(1);
            f.readBytes(15);

            int32_t isgmtcnt = f.readInt32();
            int32_t isstdcnt = f.readInt32();
            int32_t leapcnt = f.readInt32();
            int32_t timecnt = f.readInt32();
            int32_t typecnt = f.readInt32();
            int32_t charcnt = f.readInt32();

            std::vector<int32_t> trans;
            std::vector<int> localtimes;
            trans.reserve(timecnt);

            for(int i=0; i<timecnt; ++i) {
                trans.push_back(f.readInt32());
            }
            for(int i=0; i<timecnt; ++i) {
                localtimes.push_back(f.readUint8());
            }
            for(int i=0; i<typecnt; ++i) {
                int32_t gmtoff = f.readInt32();
                uint8_t isdst = f.readUint8();
                uint8_t abbrind = f.readUint8();
                data->localtimes.push_back(Localtime(gmtoff, isdst, abbrind));
            }
            for(int i=0; i<timecnt; ++i) {
                int localIdx = localtimes[i];
                time_t localtime = trans[i] + data->localtimes[localIdx].gmtOffset;
                data->transitions.push_back(Transition(trans[i], localtime ,localIdx));
            }
            data->abbreviation = f.readBytes(charcnt);
            
            (void)isgmtcnt;
            (void)isstdcnt;
            (void)leapcnt;
        }
        catch(std::logic_error& e) {
            fprintf(stderr, "%s\n", e.what());
        }
    }
    return true;
}

const Localtime* findLocaltime(const TimeZone::Data& data, Transition entry, Comp comp) {
    const Localtime* local = nullptr;
    if(data.transitions.empty() || comp(entry, data.transitions.front()))
        local = &data.localtimes.front();
    else {
        std::vector<Transition>::const_iterator iter = lower_bound(data.transitions.begin(), data.transitions.end(), entry, comp);
        if(iter != data.transitions.end()) {
            if(!comp.equal(entry, *iter)) {
                assert(iter != data.transitions.begin());
                --iter;
            }
            local = &data.localtimes[iter->localtimeIdx];
        }
        else {
            local = &data.localtimes[data.transitions.back().localtimeIdx];
        }
    }
    return local;
}


TimeZone::TimeZone(const char* zonefile) :
    _data(new TimeZone::Data) {
    if(!readTimeZoneFile(zonefile, _data.get())) {
        _data.reset();
    }
}

TimeZone::TimeZone(int eastOfUtc, const char* name) :
    _data(new TimeZone::Data) {
    _data->localtimes.push_back(Localtime(eastOfUtc, false, 0));
    _data->abbreviation = name;
}

struct tm TimeZone::toLocalTime(time_t seconds) const {
    struct tm localtime;
    memset(&localtime, 0, sizeof localtime);
    assert(_data != nullptr);
    const Data& data(*_data);

    Transition entry(seconds, 0, 0);
    const Localtime* local = findLocaltime(data, entry, Comp(true));

    if(local) {
        time_t localSeconds = seconds + local->gmtOffset;
        ::gmtime_r(&localSeconds, &localtime);
        localtime.tm_isdst = local->isDst;
        localtime.tm_gmtoff = local->gmtOffset;
        localtime.tm_zone = &data.abbreviation[local->arrbIdx];
    }
    return localtime;
}

time_t TimeZone::fromLocalTime(const struct tm& localtm) const {
    assert(_data != nullptr);
    const Data& data(*_data);

    struct tm tmp = localtm;
    time_t seconds = ::timegm(&tmp);
    Transition entry(0, seconds, 0);
    const Localtime* local = findLocaltime(data, entry, Comp(false));
    if(localtm.tm_isdst) {
        struct tm trytm = toLocalTime(seconds - local->gmtOffset);
        if(!trytm.tm_isdst && trytm.tm_hour == localtm.tm_hour && trytm.tm_min == localtm.tm_min)
            seconds -= 3600;
    }
    return seconds - local->gmtOffset;
}

struct tm TimeZone::toUtcTime(time_t secondsSinceEpoch, bool yday) {
    struct tm utc;
    memset(&utc, 0, sizeof utc);
    utc.tm_zone = "GMT";
    int seconds = static_cast<int>(secondsSinceEpoch % kSecondsPerDay);
    int days = static_cast<int>(secondsSinceEpoch / kSecondsPerDay);
    if(seconds < 0) {
        seconds += kSecondsPerDay;
        --days;
    }
    
    fillHMS(seconds, &utc);
    Date date(days + Date::kJulianDayOf1970_01_01);
    Date::YearMonthDay ymd = date.yearMonthDay();
    utc.tm_year = ymd.year - 1900;
    utc.tm_mon = ymd.month - 1;
    utc.tm_mday = ymd.day;
    utc.tm_wday = date.weekDay();

    if(yday) {
        Date startOfYear(ymd.year, 1, 1);
        utc.tm_yday = date.julianDayNumber() - startOfYear.julianDayNumber();
    }
    return utc;
}

time_t TimeZone::fromUtcTime(const struct tm& utc) {
    return fromUtcTime(utc.tm_year+1900, utc.tm_mon+1, utc.tm_mday, utc.tm_hour, utc.tm_min, utc.tm_sec);
}

time_t TimeZone::fromUtcTime(int year, int month, int day, int hour, int minute, int second) {
    Date date(year, month, day);
    int secondsInDay = hour*3600 + minute*60 + second;
    time_t days = date.julianDayNumber() - Date::kJulianDayOf1970_01_01;
    return days*kSecondsPerDay + secondsInDay; 
}


