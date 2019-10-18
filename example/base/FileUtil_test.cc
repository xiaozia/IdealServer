
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : FileUtil_test.cc
*   Last Modified : 2019-06-09 15:23
*   Describe      :
*
*******************************************************/

#include "ideal/base/FileUtil.h"

#include <stdio.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

using namespace ideal;

int main() {
	std::string result;
	int64_t size = 0;

	int err = FileUtil::readFile("/proc/self", 1024, &result, &size);
	printf("%d %zd %" PRIu64 "\n", err, result.size(), size);

	err = FileUtil::readFile("/proc/self", 1024, &result, NULL);
	printf("%d %zd %" PRIu64 "\n", err, result.size(), size);

	err = FileUtil::readFile("/proc/self/cmdline", 1024, &result, &size);
	printf("%d %zd %" PRIu64 "\n", err, result.size(), size);

	err = FileUtil::readFile("/dev/null", 1024, &result, &size);
	printf("%d %zd %" PRIu64 "\n", err, result.size(), size);

	err = FileUtil::readFile("/dev/zero", 1024, &result, &size);
	printf("%d %zd %" PRIu64 "\n", err, result.size(), size);

	err = FileUtil::readFile("/notexist", 1024, &result, &size);
	printf("%d %zd %" PRIu64 "\n", err, result.size(), size);

	err = FileUtil::readFile("/dev/zero", 102400, &result, &size);
	printf("%d %zd %" PRIu64 "\n", err, result.size(), size);

	err = FileUtil::readFile("/dev/zero", 102400, &result, NULL);
	printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
}

