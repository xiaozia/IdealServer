
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : CurrentThread.cc
*   Last Modified : 2019-11-23 14:16
*   Describe      :
*
*******************************************************/

#include "ideal/base/CurrentThread.h"
#include "ideal/base/Timestamp.h"

#include <type_traits>
#include <stdio.h>
#include <cxxabi.h>

#include <unistd.h>
#include <sys/syscall.h>
#include <execinfo.h>

namespace ideal {

namespace CurrentThread {

// t_XXX 每个线程独有
__thread int t_cachedTid = 0;
__thread char t_tidString[32];
__thread int t_tidStringLength = 6;
__thread const char* t_threadName = "Unknown";

static_assert(std::is_same<pid_t, int>::value, "pid_t should be int");

pid_t gettid() {
    return static_cast<pid_t>(::syscall(SYS_gettid));
}


void getCachedTid() {
    if(t_cachedTid == 0) {
        t_cachedTid = gettid();
        t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
    }
}

bool isMainThread() {
    return tid() == ::getpid();
}

void sleepUsec(int64_t usec) {
    struct timespec ts = { 0, 0 };
    ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicrosecondsPerSecond);
    ts.tv_nsec = static_cast<long>(usec % Timestamp::kMicrosecondsPerSecond * 1000);
    ::nanosleep(&ts, NULL);
}

std::string stackTrace(bool demangle) {
	std::string stack;
	const int max_frames = 200;
	void* frame[max_frames];
	int nptrs = ::backtrace(frame, max_frames);
	char** strings = ::backtrace_symbols(frame, nptrs);
	if(strings)
  	{
    	size_t len = 256;
    	char* demangled = demangle? static_cast<char*>(::malloc(len)) : nullptr;
    	for(int i = 1; i < nptrs; ++i)
    	{
      		if(demangle)
      		{
        		char* left_par = nullptr;
        		char* plus = nullptr;
        		for(char* p = strings[i]; *p; ++p)
        		{
          			if(*p == '(')
            			left_par = p;
          			else if(*p == '+')
            			plus = p;
        		}

        		if(left_par && plus)
        		{
          			*plus = '\0';
          			int status = 0;
          			char* ret = abi::__cxa_demangle(left_par+1, demangled, &len, &status);
          			*plus = '+';
          			if(status == 0)
          			{
            			demangled = ret;
            			stack.append(strings[i], left_par+1);
            			stack.append(demangled);
            			stack.append(plus);
            			stack.push_back('\n');
            			continue;
          			}
        		}
      		}
    		stack.append(strings[i]);
    		stack.push_back('\n');
		}
    	free(demangled);
    	free(strings);
  	}
  	return stack;
}

}

}

