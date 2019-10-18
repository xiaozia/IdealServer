
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : ProcessInfo_test.cc
*   Last Modified : 2019-06-09 22:21
*   Describe      :
*
*******************************************************/

#include "ideal/base/ProcessInfo.h"
#include <stdio.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

int main() {
	printf("pid = %d\n", ideal::ProcessInfo::pid());
	printf("uid = %d\n", ideal::ProcessInfo::uid());
	printf("euid = %d\n", ideal::ProcessInfo::euid());
	printf("start time = %s\n", ideal::ProcessInfo::startTime().toFormattedString().c_str());
	printf("hostname = %s\n", ideal::ProcessInfo::hostname().c_str());
	printf("opened files = %d\n", ideal::ProcessInfo::openedFiles());
	printf("threads = %zd\n", ideal::ProcessInfo::threads().size());
	printf("num threads = %d\n", ideal::ProcessInfo::numThreads());
	printf("status = %s\n", ideal::ProcessInfo::procStatus().c_str());
}

