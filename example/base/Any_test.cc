
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : Any_test.cc
*   Last Modified : 2019-11-06 20:33
*   Describe      :
*
*******************************************************/

#include "ideal/base/Any.h"

#include <iostream>
#include <string>

#include <gtest/gtest.h>

using namespace ideal;

TEST(Any, testAny) {
    Any a = 31415;
    Any b = std::string("hello world");
    Any c;

    EXPECT_FALSE(a.empty());
    EXPECT_FALSE(b.empty());
    EXPECT_TRUE(c.empty());

    EXPECT_TRUE(a.isType<int>());
    EXPECT_FALSE(a.isType<std::string>());
    EXPECT_EQ(a.any_cast<int>(), 31415);
    
    c = b;
    EXPECT_FALSE(c.isType<int>());
    EXPECT_TRUE(c.isType<std::string>());
    EXPECT_TRUE(c.isType<const std::string&>());
    EXPECT_TRUE(c.isType<std::string&&>());
    EXPECT_EQ(c.any_cast<std::string>(), std::string("hello world"));
//    std::cout << c.any_cast<int>() << std::endl;
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

