
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Exception_test.cc
*   Last Modified : 2019-05-26 15:44
*   Describe      :
*
*******************************************************/

#include "ideal/base/CurrentThread.h"
#include "ideal/base/Exception.h"

#include <functional>
#include <vector>
#include <cstdio>

class Bar {
public:
	void test(std::vector<std::string> names = {}) {
		printf("Stack:\n%s\n", ideal::CurrentThread::stackTrace(true).c_str());
    	
		[]{
      		printf("Stack inside lambda:\n%s\n", ideal::CurrentThread::stackTrace(true).c_str());
    	}(); // 匿名
    
		std::function<void()> func([]{
      		printf("Stack inside std::function:\n%s\n", ideal::CurrentThread::stackTrace(true).c_str());
    	});
    	func();

    	func = std::bind(&Bar::callback, this);
    	func();

    	throw ideal::Exception("oops");
  }

private:
	void callback() {
   		printf("Stack inside std::bind:\n%s\n", ideal::CurrentThread::stackTrace(true).c_str());
	}
};


void foo() {
	Bar b;
	b.test();
}


int main()
{
	try {
		foo();
  	}
  	catch(const ideal::Exception& ex) {
    	printf("reason: %s\n", ex.what());
    	printf("stack trace:\n%s\n", ex.stackTrace());
  	}
}


