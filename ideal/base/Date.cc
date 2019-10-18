
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Date.cc
*   Last Modified : 2019-06-04 19:42
*   Describe      :
*
*******************************************************/

#include "ideal/base/Date.h"

using namespace ideal;

int getJulianDayNumber(int year, int month, int day) {
    int a = (14 -month) / 12;
    int y = year + 4800 - a;
    int m = month + 12 * a - 3;
    return day + (153*m + 2) / 5 + y*365 + y/4 - y/100 + y/400 -32045;
}

Date::YearMonthDay getYearMonthDay(int julianDayNumber) {
    int a = julianDayNumber + 32044;
    int b = (4 * a + 3) / 146097;
    int c = a - ((b * 146097) / 4);
    int d = (4 * c + 3) / 1461;
    int e = c - ((1461 * d) / 4);
    int m = (5 * e + 2) / 153;
	Date::YearMonthDay ymd;
	ymd.day = e - ((153 * m + 2) / 5) + 1;
	ymd.month = m + 3 - 12 * (m / 10);
	ymd.year = b * 100 + d - 4800 + (m / 10);
	return ymd;
}

const int Date::kJulianDayOf1970_01_01 = getJulianDayNumber(1970, 1, 1);

Date::Date(int year, int month, int day) :
    _julianDayNumber(getJulianDayNumber(year, month ,day)) {
}

Date::Date(const struct tm& t) :
    _julianDayNumber(getJulianDayNumber(t.tm_year+1900, t.tm_mon+1, t.tm_mday)) {    
}

std::string Date::toString() const {
    char buf[32] = { 0 };
	snprintf(buf, sizeof buf, "%4d-%02d-%02d", year(), month(), day());
	return buf;
}
    
Date::YearMonthDay Date::yearMonthDay() const {
    return getYearMonthDay(_julianDayNumber);
}


