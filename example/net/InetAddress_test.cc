
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : InetAddress_test.cc
*   Last Modified : 2019-11-05 20:30
*   Describe      :
*
*******************************************************/

#include "ideal/net/InetAddress.h"
#include "ideal/base/Logger.h"

#include <gtest/gtest.h>

using ideal::net::InetAddress;

TEST(InetAddress, testInetAddress) {
	InetAddress addr0(1234);
	EXPECT_EQ(addr0.toIp(), std::string("0.0.0.0"));
	EXPECT_EQ(addr0.toIpPort(), std::string("0.0.0.0:1234"));
	EXPECT_EQ(addr0.toPort(), 1234);

	InetAddress addr1(4321, true);
	EXPECT_EQ(addr1.toIp(), std::string("127.0.0.1"));
	EXPECT_EQ(addr1.toIpPort(), std::string("127.0.0.1:4321"));
    EXPECT_EQ(addr1.toPort(), 4321);

	InetAddress addr2("1.2.3.4", 8888);
	EXPECT_EQ(addr2.toIp(), std::string("1.2.3.4"));
	EXPECT_EQ(addr2.toIpPort(), std::string("1.2.3.4:8888"));
	EXPECT_EQ(addr2.toPort(), 8888);

	InetAddress addr3("255.254.253.252", 65535);
	EXPECT_EQ(addr3.toIp(), std::string("255.254.253.252"));
	EXPECT_EQ(addr3.toIpPort(), std::string("255.254.253.252:65535"));
	EXPECT_EQ(addr3.toPort(), 65535);
}

TEST(InetAddress, testInetAddressResolve) {
	InetAddress addr(80);
	EXPECT_TRUE(InetAddress::resolve("baidu.com", &addr));
	EXPECT_TRUE(InetAddress::resolve("google.com", &addr));
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

