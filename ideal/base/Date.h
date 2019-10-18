
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Date.h
*   Last Modified : 2019-06-04 18:01
*   Describe      :
*
*******************************************************/

#ifndef  _IDEAL_BASE_DATE_H
#define  _IDEAL_BASE_DATE_H

#include "ideal/base/copyable.h"
#include <string>

namespace ideal {
    
class Date : public copyable {
public:
    struct YearMonthDay {
        int year;
        int month;
        int day;
    };

    static const int kDaysPerWeek = 7;
    static const int kJulianDayOf1970_01_01;

    Date() : _julianDayNumber(0) { }
    Date(int year, int month, int day);
    explicit Date(int julianDayNumber) : _julianDayNumber(julianDayNumber) { }
    explicit Date(const struct tm&);

    void swap(Date& that) {
        std::swap(_julianDayNumber, that._julianDayNumber);
    }
    bool vaild() const { return _julianDayNumber > 0; }
    std::string toString() const;
    
    YearMonthDay yearMonthDay() const;
    int year() const { return yearMonthDay().year; }
    int month() const { return yearMonthDay().month; }
    int day() const { return yearMonthDay().day; }

    int weekDay() const { return (_julianDayNumber + 1) % kDaysPerWeek; }
    int julianDayNumber() const { return _julianDayNumber; }

private:
    int _julianDayNumber;
};

inline bool operator<(const Date& x, const Date& y) {
    return x.julianDayNumber() < y.julianDayNumber();
}

inline bool operator==(const Date& x, const Date& y) {
    return x.julianDayNumber() == y.julianDayNumber();
}

}

#endif // _IDEAL_BASE_DATE_H


