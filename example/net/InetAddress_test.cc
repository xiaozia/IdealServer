
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : InetAddress_test.cc
*   Last Modified : 2019-06-10 18:07
*   Describe      :
*
*******************************************************/

#include "ideal/net/InetAddress.h"
#include "ideal/base/Logger.h"

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using ideal::net::InetAddress;

BOOST_AUTO_TEST_CASE(testInetAddress) {
	InetAddress addr0(1234);
	BOOST_CHECK_EQUAL(addr0.toIp(), std::string("0.0.0.0"));
	BOOST_CHECK_EQUAL(addr0.toIpPort(), std::string("0.0.0.0:1234"));
	BOOST_CHECK_EQUAL(addr0.toPort(), 1234);

	InetAddress addr1(4321, true);
	BOOST_CHECK_EQUAL(addr1.toIp(), std::string("127.0.0.1"));
	BOOST_CHECK_EQUAL(addr1.toIpPort(), std::string("127.0.0.1:4321"));
	BOOST_CHECK_EQUAL(addr1.toPort(), 4321);

	InetAddress addr2("1.2.3.4", 8888);
	BOOST_CHECK_EQUAL(addr2.toIp(), std::string("1.2.3.4"));
	BOOST_CHECK_EQUAL(addr2.toIpPort(), std::string("1.2.3.4:8888"));
	BOOST_CHECK_EQUAL(addr2.toPort(), 8888);

	InetAddress addr3("255.254.253.252", 65535);
	BOOST_CHECK_EQUAL(addr3.toIp(), std::string("255.254.253.252"));
	BOOST_CHECK_EQUAL(addr3.toIpPort(), std::string("255.254.253.252:65535"));
	BOOST_CHECK_EQUAL(addr3.toPort(), 65535);
}

BOOST_AUTO_TEST_CASE(testInetAddressResolve) {
	InetAddress addr(80);
	if(InetAddress::resolve("baidu.com", &addr)) {
		LOG_INFO << "baidu.com resolved to " << addr.toIpPort();
	}
	else {
		LOG_ERROR << "Unable to resolve baidu.com";
	}
}

