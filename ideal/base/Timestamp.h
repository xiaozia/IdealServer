
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Timestamp.h
*   Last Modified : 2019-05-24 21:17
*   Describe      :
*
*******************************************************/

#ifndef  _NET_TIMESTAMP_H
#define  _NET_TIMESTAMP_H

#include "copyable.h"

#include <stdint.h>
#include <string>

namespace ideal 
{

class Timestamp : public copyable {
public:
    Timestamp() :
        _microsecondsSinceEpoch(0) { 
    }

    explicit Timestamp(int64_t microsecondsSinceEpoch) :
        _microsecondsSinceEpoch(microsecondsSinceEpoch) {
    }

    void swap(Timestamp& that) {
        std::swap(_microsecondsSinceEpoch, that._microsecondsSinceEpoch);
    }

    std::string toString() const;
    std::string toFormattedString(bool showMicroseconds = true) const;

    bool valid() const { return _microsecondsSinceEpoch > 0; }

    int64_t microsecondsSinceEpoch() const {
        return _microsecondsSinceEpoch;
    }

    time_t secondsSinceEpoch() const {
        return static_cast<time_t>(_microsecondsSinceEpoch / kMicrosecondsPerSecond);
    }

    static Timestamp now();
    static Timestamp invalid() { return Timestamp(); }

    static Timestamp fromUnixTime(time_t t) {
        return Timestamp(static_cast<int64_t>(t) * kMicrosecondsPerSecond);
    }

    static const int kMicrosecondsPerSecond = 1000 * 1000;

private:
    int64_t _microsecondsSinceEpoch;
};

inline double timeDifference(const Timestamp& high, const Timestamp& low) {
    int64_t diff = high.microsecondsSinceEpoch() - low.microsecondsSinceEpoch();
    return static_cast<double>(diff) / Timestamp::kMicrosecondsPerSecond;
}

// Timer.cc
inline Timestamp addTime(Timestamp timestamp, double seconds) {
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicrosecondsPerSecond);
    return Timestamp(timestamp.microsecondsSinceEpoch() + delta);
}

inline bool operator<(Timestamp lhs, Timestamp rhs) {
    return lhs.microsecondsSinceEpoch() < rhs.microsecondsSinceEpoch();
}

inline bool operator==(Timestamp lhs, Timestamp rhs) {
    return lhs.microsecondsSinceEpoch() == rhs.microsecondsSinceEpoch();
}

}

#endif // _NET_TIMESTAMP_H


