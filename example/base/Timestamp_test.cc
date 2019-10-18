
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Timestamp_test.cc
*   Last Modified : 2019-05-25 18:24
*   Describe      :
*
*******************************************************/

#include "ideal/base/Timestamp.h"

#include <vector>
#include <cstdio>

using namespace ideal;

void passByConstReference(const Timestamp& x)
{
	printf("%s\n", x.toString().c_str());
}

void passByValue(Timestamp x)
{
	printf("%s\n", x.toString().c_str());
}

void benchmark()
{
	const int kNumber = 1000*1000;

	std::vector<Timestamp> stamps;
	stamps.reserve(kNumber);
	for(int i = 0; i < kNumber; ++i)
	{
    	stamps.push_back(Timestamp::now());
	}

	printf("%s\n", stamps.front().toString().c_str());
	printf("%s\n", stamps.back().toString().c_str());
	printf("%f\n", timeDifference(stamps.back(), stamps.front()));

	int increments[100] = { 0 };
	int64_t start = stamps.front().microsecondsSinceEpoch();
	for(int i = 1; i < kNumber; ++i)
  	{
    	int64_t next = stamps[i].microsecondsSinceEpoch();
    	int64_t inc = next - start;
    	start = next;
    	if (inc < 0)
    	{
      		printf("reverse!\n");
    	}
    	else if (inc < 100)
    	{
      		++increments[inc];
    	}
    	else
    	{
      		printf("big gap %d\n", static_cast<int>(inc));
    	}
	}
  	for(int i = 0; i < 100; ++i)
  	{
    	printf("%2d: %d\n", i, increments[i]);
  	}
}

int main()
{
	Timestamp now(Timestamp::now());
	printf("%s\n", now.toString().c_str());
    printf("%s\n", now.toFormattedString().c_str());
  
	passByValue(now);
 	passByConstReference(now);
 	benchmark();
}


