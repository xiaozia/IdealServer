
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : LogFile_test.cc
*   Last Modified : 2019-06-10 17:56
*   Describe      :
*
*******************************************************/

#include "ideal/base/LogFile.h"
#include "ideal/base/Logger.h"

#include <unistd.h>

std::unique_ptr<ideal::LogFile> g_logFile;

void outputFunc(const char* msg, int len) {
	g_logFile->append(msg, len);
}

void flushFunc() {
	g_logFile->flush();
}

int main(int argc, char* argv[]) {
	char name[256] = { 0 };
	strncpy(name, argv[0], sizeof name - 1);
	g_logFile.reset(new ideal::LogFile(::basename(name), 200*1000));

	ideal::Logger::setOutput(outputFunc);
	ideal::Logger::setFlush(flushFunc);

	std::string line = "1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

	for(int i = 0; i < 10000; ++i) {
		LOG_INFO << line << i;
		usleep(1000);
    }
}

